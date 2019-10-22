#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#include "RuderwegMessSensorTypes.h"
#include "htmlRootPage.h"
#include "htmlAdminPage.h"
#include "htmlScript.h"
#include "htmlCSS.h"

#include <Adafruit_MMA8451.h>         // MMA8451 library

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Version history
// V0.10 : full functional initial version
// V0.11 : use wifi data from private header file
// V0.12 : support for MMA8451 added (no test HW available)
// V0.13 : support for OLED
// V0.14 : displayed angle with more precision
// V0.15 : multi WiFi mode added, in addition to client mode
//         the sensor now works as AP with
// V0.16 : extract individual #define to mySettings.h
//         and do some function sorting
// V0.17 : admin page added, for individual settings support,
//         added support for precision and inverted values,
//         including persistent config support (EEPROM),
//         refactoring of AJAX procedures
// V0.18 : bugfix : wrong calculation of rudderdepth from web GUI
// V0.19 : more bugfix : wifi, config page, new amplitude prec 0.01mm
// V0.20 : bugfix : wrong setting of amplitude presision in admin page
// V0.21 : show ip address of wlan in admin page,
// V0.22 : fixed handling of starting AP, more client details in serial logging
// V0.23 : support for reset to default config when connecting D5 with GRD while startup
//         support flight phase supporting measure with neutral, min, max values
//         CSS based layout added
// V0.24 : automatic detection of I2C ports and sensor type
// V0.25 : added calibration of MPU6050 in admin page and add values to persitent config data
//         after calibration the MPU6050 is accurate < 0.5° at a range of +-45°
// V0.26 : support for measure of rudder amplitude arc / chord / vertical distance - configurable
// V0.27 : typos and coding bug in initialization of MPU6050
// V0.28 : wording, and inline documentation
// V0.29 : avoid timing problems when restart response to client is send
// V0.30 : support for different I2C addresses and better support for MMA8451 added, refactoring of sensor initialization
//         for this the Adafruit_MMA8451 library is patched and this fork [https://github.com/Pulsar07/Adafruit_MMA8451_Library] has to be used
// V0.31 : enhancedd MPU6050 calibration
// V0.32 : assuming MM8452 if I2C address of MMA8451 is used but sensor ID is not as expected
// V0.33 : typo in MM8452 detection
#define WM_VERSION "V0.33"

/**
 * \file RuderwegMessSensor.ino
 *
 * \brief small tool to measure the model glider rudders movement
 *
 * \author Author: Rainer Stransky
 *
 * \copyright This project is released under the GNU Public License v3
 *          see https://www.gnu.org/licenses/gpl.html.
 * Contact: opensource@so-fa.de
 *
 */

/**
 * \mainpage RuderwegMessSensor, ein Arduino WeMos D1 Mini Projekt zur Erstellung eines Ruderweg Mess Sensors
 *
 * \section intro_sec_de Übersicht
 * Sensor zum Messen von Ruderwinkel und Ruderweg an Modellflugzeugen und Anzeige auf Web-Seite.
 * Hier ein kurzer Überblick über die Komponenten des Sensors.
 *  Der Sensor besteht aus der GY-521/MPU6050 Sensorplatine, die über eine I2C-Bus mit der
 * Mikrokontrollerplatine Wemos D1 verbunden ist. Der Mikrokontroller liest die Werte des Sensor
 * macht die Berechnungen, stellt einen Web-Server über eine WiFi-Schnittstelle zur Verfügung,
 * über die sowohl die Messdaten als auch die Konfiguration durchgeführt werden kann.
 * ![Architektur](https://raw.githubusercontent.com/Pulsar07/RuderwegMessSensor/master/doc/img/RWMS_Architecture.png)
 *
 * \section hardware_sec_de Hardware
 * \subsection hardware_subsec_de_ms Messsensor
 * Als Messsensor wird der GY-521/MPU-6050 benutzt. Die Genauigkeit liegt nach Kalibrierung bei
 * Winkeln bis +/- 45° kleiner als 0.5°. Der Baustein MPU-6050 wird von einer wirklich sehr gut
 * gemachten Libs von J.Rowberg unterstützt (siehe Link)
 *
 * Hier ein paar Links:
 * * https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
 * * https://www.az-delivery.de/products/gy-521-6-achsen-gyroskop-und-beschleunigungssensor
 * * https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
 *
 * \subsection hardware_subsec_de_mk Mikrokontroller
 * Als Mikrokontroller wird der Wemos D1/ESP8266 benutzt, der ausreichende Rechenpower und
 * Speicherresourcen bietet und eine WLAN Schnittstelle hat. Auch dieser Mikrokontroller
 * hat super gemachte Bibliotheken, zur Nutzung der WiFi-Schnittstelle und zur Programmierung
 * des Web-Servers.
 *
 * Hier ein paar Links:
 * * https://www.az-delivery.de/products/d1-mini
 * * https://github.com/esp8266/Arduino
 *
 * \subsection hardware_subsec_de_sp Schaltplan
 * Der Schaltplan ist denkbar einfach. Es werden nur 4 Verbindungen zwischen Sensorplatine und
 * Mikrokontroller benötigt. Das Layout und die Software sind so ausgelegt, dass mit einer Stiftleiste
 * 4x1 das Sensorboard mit dem Gesicht in Richtung Mikrokontroller direkt verlötet werden kann.
 * ![Schaltplan](https://raw.githubusercontent.com/Pulsar07/RuderwegMessSensor/master/doc/img/RWMS_Schaltplan.png)
 *
 * \section hmi_sec_de Anleitung
 * \subsection hmi_subsec_de_ib Inbetriebnahme
 * * Stromversorgung
 *  * Der Sensor / das Mikroprozessorboard ist mit einem Micro-USB-Anschluss ausgestattet,
 * hier kann man jedes handelsübliche USB-Netzteil anschließen oder besser jede
 * normale Powerbank. Damit ist man in der Werkstatt oder auf dem Flugfeld mobil ausgestattet.
 * * WiFi
 *  * Der Sensor muss zuerst mit Smartphone oder PC verbunden werden. Dazu stellt
 * der Sensor per WiFi einen Accesspoint mit der SSID "UHU" und Kennwort "12345678"
 * zur Verfügung. Ist das Gerät mit diesem WLAN verbunden, kann im Web-Browser
 * über die Adresse http://192.168.4.1 die Benutzeroberfläche benutzt und der Sensor
 * konfiguriert werden. Sowohl obige SSID als auch das Kennwort können danach geändert werden.
 *  * Auf der Einstellseite kann eine SSID und ein Kennwort für ein WLAN (WLAN-Client)
 * konfiguriert werden, mit dem sich der Sensor verbinden. Dabei wird dem Sensor
 * eine IP-Adresse zugewiesen, die am WLAN-Router abgefragt werden muss. Änderungen
 * der WLAN Einstellungen müssen gespeichert werden und werden erst nach Neustart aktiv.
 *  * Ist die Verbindung zu einem WLAN konfiguriert (WLAN-Client), kann auf der
 * Einstellungsseite, der Accesspoint deaktiviert werden (nach Speichern und Neustart).
 * Kann beim Neustart keine Verbindung zum konfigurierten WLAN aufgebaut werden,
 * wird der Accesspoint-Mode aber trotzdem aktiviert, damit ein Zugang zum Gerät möglich ist.
 * * Nutzung des Sensorboard GY-521 mit MPU-6050
 *  * Genauigkeit: Der MEMS Chip des MPU-6050 sollte Winkelauflösungen besser als 0.5°
 * bei 45° Ausschlag messen können, was bei einer 60mm Rudertiefe von 60mm einen Fehler von kleiner als 0.5mm ergibt.
 *  * Kalibrierung: Damit der MPU-6050 allerdings diese Genauigkeit erreicht, muss
 * er nachträglich kalibriert werden. Die Software unterstützt diese Funktion und
 * kann die Werte intern speichern. Zur Kalibrierung muss die GY-521-Sensorplatine
 * mit der flachen Rückseite möglichst exakt horizontal aufgelegt werden. Dann den
 * Kalibrier-Button drücken und ca. 5s warten bis die Kalibrierung beendet ist.
 * Dabei sollte die Auflagefläche (Tisch) nicht bewegt werden und frei von Vibrationen sein.
 *  * Einbaulage: Die Sensorplatine sollte auch genau so, wie bei der Kalibrierung,
 * betrieben werden. Also die flache Seite nach unten und die Seite mit den
 * Elektronikbausteinen nach oben. Nur so wird die oben genannte Genauigkeit erreicht.
 *  * Achsen und Anzeige-Genauigkeit: Auf der Konfigurationsseite, kann die Bezugs-Achse
 * der Winkelmessung, je nach Einbaulage in der Klemmeinrichtung ausgewählt werden.
 * Zudem sind diverse Anzeigegenauigkeiten für die Winkel und die Ruderwegs-Messung auswählbar.
 * Die Anzeige hat zwar immer auf 2 Dezimalstellen, aber intern wird dann gerundet.
 *
 * \subsection hmi_subsec_de_me  Messen
 * * Der mit dem Mikrokontroller verbundene Messensor sollte mit einer Klemmvorrichtung fest
 * verbunden sein, und kann dann einfach an eine beliebige Stelle des Ruders aufgeklemmt werden.
 * Die Ruderdrehachse, sollte möglichst parallel zur ausgewählten Dreh-Achse (X- oder Y-Achse)
 * sein. Wie schon beschrieben, muss der Sensor mit dem Gesicht nach oben auf dem Ruder
 * befestigt sein.
 * * Einschränkungen: Der Sensor kann nur Winkel in Bezug auf die Schwerkraft messen.
 * Somit sind Ruderwegsmessungen für das Seitenruder nur möglich wenn der Rumpf um
 * 90° gedreht liegt.
 * * Der Ruderweg ist abhänig von der Rudertiefe. Diese ist an der Stelle zu Messen,
 * an der man den Ruderweg messen will. In der Web-Oberfläche des Sensor kann
 * diese Rudertiefe eingegeben werden.
 * * Ist der Sensor so auf dem Ruder angebracht, und die Rudertiefe eingestellt,
 * ist die Ruderstellung in die Null-Lage zu bringen. Jetzt können Winkel
 * und Ruderweg per "Tara"-Button auf 0 gesetzt werden.
 * * Bewegt man das Ruder nun nach oben oder unten werden die Ausschläge in Grad und
 * Millimeter angezeigt. Sollte das Vorzeichen nicht den Erwartungen entsprechen,
 * kann dies bei den Einstellungen angepasst werden.
 * * Zur Flugphasenmessung kann die Min-/Max-Rudermessung benutzt werden.
 * Hier sollte man das Ruder in die Neutralstellung der Flugphase bringen.
 * Nun den Schalter für die Min-/Max-Ruderweg-Messung aktivieren. Damit wird der
 * aktuelle Ruderausschlag als Neutralwert übernommen. Jetzt können die beiden
 * Min-/Max-Werte angefahren werden. Alle drei Werte werden bis zur
 * Deaktiverung der Messung angezeigt.
 *
 * Weitere Details gibt es unter <a href="http://www.so-fa.de/nh/RuderwegMesssensor">Albatross, Seite für Modellflug und Technik</a>
 */


static Adafruit_MMA8451 mma;
static MPU6050 mpu;


#ifdef SUPPORT_OLED
#include <U8g2lib.h>         // OLED library  https://github.com/olikraus/u8g2
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D1, /* data=*/ D2); //OLED am Wemos D1 mini Pin 1 und 2
#endif

static configData_t ourConfig;

static const uint8_t MPU6050ADDR1 = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
static const uint8_t MPU6050ADDR2 = 0x69; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
static const uint8_t MMA8451ADDR1 = 0x1C; // I2C address of the MMA-8451. If AD0 pin is set to LOW, the I2C address will be 0x1C.
static const uint8_t MMA8451ADDR2 = 0x1D; // I2C address of the MMA-8451. If AD0 pin is set to LOW, the I2C address will be 0x1C.
static uint8_t ourSCL_Pin;
static uint8_t ourSDA_Pin;
static uint8_t ourI2CAddr;
static String ourSensorTypeName;
static boolean ourTriggerCalibrateMPU6050 = false;
static unsigned long ourTriggerRestart = 0;

int16_t ourAccelerometer_x, ourAccelerometer_y, ourAccelerometer_z; // variables for ourAccelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
static double ourTara = 0;
static double ourSmoothedAngle_x = 0;
static double ourSmoothedAngle_y = 0;
static double ourSmoothedAngle_z = 0;
static double ourTaraAngle_x = 0;
static double ourTaraAngle_y = 0;
static double ourTaraAngle_z = 0;
static double ourSmoothedGyro_x = 0;
static double ourSmoothedGyro_y = 0;
static double ourSmoothedGyro_z = 0;
static double ourTaraGyro_x = 0;
static double ourTaraGyro_y = 0;
static double ourTaraGyro_z = 0;
static double ourRudderDepth = 30;

static float ourNullAmpl;
static float ourMinAmpl;
static float ourMaxAmpl;
static boolean ourIsMeasureActive=false;

const char* ap_ssid = "UHU";

ESP8266WebServer server(80);    // Server Port  hier einstellen

void setup()
{
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print("Starting RuderwegMessSensor :");
  Serial.println(WM_VERSION);

  // check HW Pin 4 for HW Reset
  checkHWReset(D5);

  loadConfig();
  showConfig("stored configuration:");

  detectSensor();

  if (isI2C_MPU6050Addr()) {
    Wire.begin(ourSDA_Pin, ourSCL_Pin); //SDA, SCL
    initMPU5060();
  } else if (isI2C_MMA8451Addr()) {
    initMMA8451();
  }

  #ifdef SUPPORT_OLED
    u8g2.begin();
  #endif

  setupWiFi();
  setupWebServer();
}

void loop()
{
  static unsigned long last = 0;
  server.handleClient();

  readMotionSensor();
  doAsync();
  if ( (millis() - last) > 1000) {
    prepareMotionData();
    #ifdef SUPPORT_OLED
    setOLEDData();
    #endif
    last = millis();
  }
}


// =================================
// sensor data processing functions
// =================================
void readMotionSensor() {

  if (isI2C_MPU6050Addr()) {
    mpu.getMotion6(&ourAccelerometer_x, &ourAccelerometer_y, &ourAccelerometer_z, &gyro_x, &gyro_y, &gyro_z);
  } else
  if (isI2C_MMA8451Addr()) {
    mma.read();
    ourAccelerometer_x = mma.x;
    ourAccelerometer_y = mma.y;
    ourAccelerometer_z = mma.z;
  }

  const static double smooth = 0.98d;
  // print out data
  // Serial.println(ourAccelerometer_x);
  ourSmoothedAngle_x = irr_low_pass_filter(ourSmoothedAngle_x,
    atan2(ourAccelerometer_y, ourAccelerometer_z) * 180 / M_PI, smooth);
  ourSmoothedAngle_y = irr_low_pass_filter(ourSmoothedAngle_y,
    atan2(ourAccelerometer_z, ourAccelerometer_x) * 180 / M_PI, smooth);
  ourSmoothedAngle_z = irr_low_pass_filter(ourSmoothedAngle_z,
    atan2(ourAccelerometer_x, ourAccelerometer_y) * 180 / M_PI, smooth);
  ourSmoothedGyro_x = irr_low_pass_filter(ourSmoothedGyro_x, gyro_x, smooth);
  ourSmoothedGyro_y = irr_low_pass_filter(ourSmoothedGyro_y, gyro_y, smooth);
  ourSmoothedGyro_z = irr_low_pass_filter(ourSmoothedGyro_z, gyro_z, smooth);
}

void prepareMotionData() {
  double effAngle_x = ourSmoothedAngle_x - ourTaraAngle_x;
  double effAngle_y = ourSmoothedAngle_y - ourTaraAngle_y;
  double effAngle_z = ourSmoothedAngle_z - ourTaraAngle_z;
  double effGyro_x = ourSmoothedGyro_x - ourTaraGyro_x;
  double effGyro_y = ourSmoothedGyro_y - ourTaraGyro_y;
  double effGyro_z = ourSmoothedGyro_z - ourTaraGyro_z;
  // Serial.print(String("WX = ") + roundToDot5(effAngle_x));
  // Serial.print(String(" WY = ") + roundToDot5(effAngle_y));
  // Serial.print(String(" WZ = ") + roundToDot5(effAngle_z));
  // Serial.print(String(" GX = ") + roundToDot5(effGyro_x));
  // Serial.print(String(" GY = ") + roundToDot5(effGyro_y));
  // Serial.print(String(" GZ = ") + roundToDot5(effGyro_z));
  // Serial.println();
}

double getAngle() {
  double theAngle = 0;
  switch (ourConfig.axis) {
    case xAxis:
      theAngle = ourSmoothedAngle_x;
    break;
    case yAxis:
      theAngle = ourSmoothedAngle_y;
    break;
    case zAxis:
      theAngle = ourSmoothedAngle_z;
    break;
  }
  // support range -180 - +180, independent from tara
  if (theAngle < (ourTara-180.0d)) {
    theAngle = theAngle + 360.0d;
  }
  return (theAngle - ourTara) * ourConfig.angleInversion;
}

double getAmplitude(double aAngle) {
  double res;
  if (ourConfig.amplitudeCalcMethod == CHORD) {
    // sin (angle/2) = sin(angle/2 *M_PI/180)
    res = sin(aAngle*M_PI/360) * 2 * ourRudderDepth * ourConfig.amplitudeInversion;
  } else if (ourConfig.amplitudeCalcMethod == VERTICAL_DISTANCE) {
    res =  sin(aAngle*M_PI/180) * ourRudderDepth * ourConfig.amplitudeInversion;
  } else {
    res =  (aAngle/180*M_PI * ourRudderDepth) * ourConfig.amplitudeInversion;
  }
  return res;
}


float getRoundedAngle() {
  return roundPrecision(getAngle(), ourConfig.anglePrecision);
}

float getRoundedAmplitude() {
  return roundPrecision(getAmplitude(getAngle()), ourConfig.amplitudePrecision);
}

float getRudderAmplitude() {
  float ampl = getRoundedAmplitude();
  if (ourIsMeasureActive) {
    ourMinAmpl = min(ourMinAmpl, ampl);
    ourMaxAmpl = max(ourMaxAmpl, ampl);
  }
  return ampl;
}

float roundPrecision(double aVal, precision_t aPrecision) {
  float res = aVal;
  switch(aPrecision) {
    case P001:
      break;
    case P010:
      res = round(res * 10)/10;
      break;
    case P050:
      res = round(res * 2)/2;
      break;
    case P100:
      res = round(res);
      break;
    }
  return res;
}

void taraAngle() {
  ourTaraAngle_x = ourSmoothedAngle_x;
  ourTaraAngle_y = ourSmoothedAngle_y;
  ourTaraAngle_z = ourSmoothedAngle_z;
  switch (ourConfig.axis) {
    case xAxis:
      ourTara = ourSmoothedAngle_x;
    break;
    case yAxis:
      ourTara = ourSmoothedAngle_y;
    break;
    case zAxis:
      ourTara = ourSmoothedAngle_z;
    break;
  }
  Serial.println("tara angle set to :" + String(ourTara));
}

void flightPhaseMeasure(boolean aStart) {
  Serial.println(String("flightPhaseMeasure(")+aStart+")");
  if (aStart) {
    ourNullAmpl = getRudderAmplitude();
    ourMinAmpl = ourMaxAmpl = ourNullAmpl;
    ourIsMeasureActive=true;
  } else {
    ourIsMeasureActive=false;
  }
}

// =================================
// web server functions
// =================================

void setupWebServer() {
  // react on these "pages"
  server.on("/",HTMLrootPage);
  server.on("/adminPage",HTMLadminPage);
  server.on("/getDataReq",getDataReq);
  server.on("/setDataReq",setDataReq);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  server.begin();               // Starte den Server
  Serial.println("HTTP Server gestartet");
}

void HTMLrootPage() {
  Serial.print(server.client().remoteIP().toString());
  Serial.println(" : HTMLrootPage()");
  checkHTMLArguments();
  String s = FPSTR(MAIN_page); //Read HTML contents
  s.replace("###<SCRIPT>###", FPSTR(SCRIPT));
  s.replace("###<CSS>###", FPSTR(CSS));
  server.send(200, "text/html", s); //Send web page
}

void HTMLadminPage() {
  Serial.print(server.client().remoteIP().toString());
  Serial.println(" : HTMLadminPage()");
  String s = FPSTR(ADMIN_page); //Read HTML contents
  s.replace("###<SCRIPT>###", FPSTR(SCRIPT));
  s.replace("###<CSS>###", FPSTR(CSS));
  server.send(200, "text/html", s); //Send web page
}


void setDataReq() {
  Serial.print(server.client().remoteIP().toString());
  Serial.println(" : setDataReq()");
  String name = server.arg("name");
  String value = server.arg("value");
  Serial.print("  "); Serial.print(name); Serial.print("="); Serial.println(value);
  boolean sendResponse = true;

  String retVal = "";
  int htmlResponseCode=200; // OK
  if ( name == "cmd_taraAngle") {
    taraAngle();
  } else
  if ( name == "cmd_flightphaseActive") {
    if (value == "true") {
      flightPhaseMeasure(true);
    } else {
      flightPhaseMeasure(false);
    }
  } else
  if ( name == "id_rudderDepth") {
    ourRudderDepth = double(atoi(value.c_str()))/10;
    Serial.println("setting rudderdepth: " + String(ourRudderDepth));
  } else
  if ( name == "id_invertAngle") {
    if (value == "true") {
      ourConfig.angleInversion = -1;
    } else {
      ourConfig.angleInversion = 1;
    }
    Serial.println("setting angle factor: " + String(ourConfig.angleInversion));
  } else
  if ( name == "id_invertAmplitude") {
    if (value == "true") {
      ourConfig.amplitudeInversion = -1;
    } else {
      ourConfig.amplitudeInversion = 1;
    }
    Serial.println("setting amplitude factor: " + String(ourConfig.amplitudeInversion));
  } else
  if ( name == "id_apActive") {
    if (value == "true") {
      ourConfig.apIsActive = true;
    } else {
      ourConfig.apIsActive = false;
    }
    Serial.println("setting AP active : " + String(ourConfig.apIsActive));
  } else
  if ( name == "nm_referenceAxis") {
    if (value == "xAxis") {
      ourConfig.axis = xAxis;
    } else if (value == "yAxis") {
      ourConfig.axis = yAxis;
    } else if (value == "zAxis") {
      ourConfig.axis = zAxis;
    }
    Serial.println("setting angle reference axis: " + String(ourConfig.axis));
  } else
  if ( name == "nm_anglePrecision") {
    if (value == "P010") {
      ourConfig.anglePrecision = P010;
    } else if (value == "P001") {
      ourConfig.anglePrecision = P001;
    }
    Serial.println("setting angle precision: " + String(ourConfig.anglePrecision));
  } else
  if ( name == "nm_precisionAmplitude") {
    if (value == "P001") {
      ourConfig.amplitudePrecision = P001;
    } else if (value == "P010") {
      ourConfig.amplitudePrecision = P010;
    } else if (value == "P050") {
      ourConfig.amplitudePrecision = P050;
    } else if (value == "P100") {
      ourConfig.amplitudePrecision = P100;
    }
    Serial.println("setting amplitude precision: " + String(ourConfig.amplitudePrecision));
  } else
  if ( name == "nm_distancetype") {
    if (value == "arc") {
      ourConfig.amplitudeCalcMethod = ARC;
    } else if (value == "chord") {
      ourConfig.amplitudeCalcMethod = CHORD;
    } else if (value == "vertical_distance") {
      ourConfig.amplitudeCalcMethod = VERTICAL_DISTANCE;
    }
    Serial.println("setting calculation method : " + value + "/" + String(ourConfig.amplitudeCalcMethod));
  } else
  if ( name == "id_wlanSsid") {
    strncpy(ourConfig.wlanSsid, value.c_str(), CONFIG_SSID_L);
    Serial.println("setting wlan ssid : " + String(ourConfig.wlanSsid));
  } else
  if ( name == "id_wlanPasswd") {
    strncpy(ourConfig.wlanPasswd, value.c_str(), CONFIG_PASSW_L);
    Serial.println("setting wlan password : " + String(ourConfig.wlanPasswd));
  } else
  if ( name == "id_apPasswd") {
    if (String(value).length() >= 8) {
      strncpy(ourConfig.apPasswd, value.c_str(), CONFIG_PASSW_L);
      Serial.println("setting AP password : " + String(ourConfig.apPasswd));
    } else {
      Serial.println("not setting AP password, too short. Old PW : " + String(ourConfig.apPasswd));
    }
  } else
  if (name == "cmd_saveConfig") {
     saveConfig();
  } else
  if (name == "cmd_resetConfig") {
     setDefaultConfig();
  } else
  if (name == "cmd_mcrestart") {
    Serial.println("resetting micro controller");
    triggerRestart();
  } else
  if (name == "cmd_calibrate") {
    if (isI2C_MPU6050Addr()) {
      Serial.println("triggering calibration");
      triggerCalibrateMPU6050();
    }
  }

  if (sendResponse) {
    Serial.print("send response to server: ");
    Serial.println(retVal);
    server.send(htmlResponseCode, "text/plane", retVal); // send an valid answer
  }
}


void getDataReq() {
  Serial.print(server.client().remoteIP().toString());
  Serial.print(" : getDataReq() :");
  String result;
  for (uint8_t i=0; i<server.args(); i++){
    // message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
    String argName = server.argName(i);

    Serial.print(argName); Serial.print(",");
    if (argName.equals("id_angleValue")) {
      result += argName + "=" + String(getRoundedAngle()) + ";";
    } else
    if (argName.equals("id_amplitudeValue")) {
      result += argName + "=" + String(getRudderAmplitude()) + ";";
    } else
    if (argName.equals("cpx_flightphase")) {
      if (ourIsMeasureActive) {
        result += String("id_rudderneutral") + "=" + String(ourNullAmpl) + ";";
        result += String("id_ruddermin" ) + "=" + String(ourMinAmpl) + ";";
        result += String("id_ruddermax" ) + "=" + String(ourMaxAmpl) + ";";
      }
    } else
    if (argName.equals("id_rudderDepth")) {
      result += argName + "=" + ourRudderDepth + ";";
    } else
    if (argName.equals("id_version")) {
      result += argName + "=" + WM_VERSION + ";";
    } else
    if (argName.equals("id_sensortype")) {
      result += argName + "=" + ourSensorTypeName + ";";
    } else
    if (argName.equals("id_wlanSsid")) {
        result += argName + "=" + ourConfig.wlanSsid + ";";
    } else
    if (argName.equals("id_wlanPasswd")) {
        result += argName + "=" + "************;";
    } else
    if (argName.equals("id_apPasswd")) {
      if (String(ourConfig.apPasswd).length() != 0) {
        result += argName + "=" + "************;";
      }
    } else
    if (argName.equals("id_invertAngle")) {
      if (ourConfig.angleInversion == -1) {
        result += argName + "=" + "checked;";
      }
    } else
    if (argName.equals("id_invertAmplitude")) {
      if (ourConfig.amplitudeInversion == -1) {
        result += argName + "=" + "checked;";
      }
    } else
    if (argName.equals("id_wlanConnetion")) {
      if (WiFi.status() == WL_CONNECTED) {
        result += argName + "=" + "verbunden, zugewiesene Adresse: " + WiFi.localIP().toString() +  ";";
      } else {
        result += argName + "=" + "nicht verbunden;";
      }
    } else
    if (argName.equals("id_resp_calibrate")) {
      if (!isSensorCalibrated()) {
        result += argName + "=" + "Sensor ist nicht kalibriert;";
      } else {
      if ( ourTriggerCalibrateMPU6050 ) {
        result += argName + "=" + "Kalibrierung gestartet ...;";
      } else {
        result += argName + "=" + "Sensor ist kalibriert;";
      }
      }
    } else
    if (argName.equals("nm_referenceAxis")) {
      switch (ourConfig.axis) {
         case xAxis:
           result += String("id_xAxis") + "=" + "checked;";
           break;
         case yAxis:
           result += String("id_yAxis") + "=" + "checked;";
           break;
         case zAxis:
           result += String("id_zAxis") + "=" + "checked;";
           break;
      }
    } else
    if (argName.equals("nm_anglePrecision")) {
      if (ourConfig.anglePrecision == P010) {
        result += String("id_anglePrec_P010") + "=" + "checked;";
      } else {
        result += String("id_anglePrec_P001") + "=" + "checked;";
      }
    } else
    if (argName.equals("nm_precisionAmplitude")) {
      if (ourConfig.amplitudePrecision == P001) {
        result += String("id_amplPrec_P001") + "=" + "checked;";
      } else if (ourConfig.amplitudePrecision == P010) {
        result += String("id_amplPrec_P010") + "=" + "checked;";
      } else if (ourConfig.amplitudePrecision == P050) {
        result += String("id_amplPrec_P050") + "=" + "checked;";
      } else if (ourConfig.amplitudePrecision == P100) {
        result += String("id_amplPrec_P100") + "=" + "checked;";
      }
    } else
    if (argName.equals("id_amplitudeCalcMethod")) {
      if (ourConfig.amplitudeCalcMethod == ARC) {
        result += argName + "=" + "Kreisbogen;";
      } else if (ourConfig.amplitudeCalcMethod == CHORD) {
        result += argName + "=" + "Kreissehne;";
      } else if (ourConfig.amplitudeCalcMethod == VERTICAL_DISTANCE) {
        result += argName + "=" + "senkrechter Abstand;";
      }
    } else
    if (argName.equals("nm_distancetype")) {
      if (ourConfig.amplitudeCalcMethod == ARC) {
        result += String("id_distance_arc") + "=" + "checked;";
      } else if (ourConfig.amplitudeCalcMethod == CHORD) {
        result += String("id_distance_chord") + "=" + "checked;";
      } else if (ourConfig.amplitudeCalcMethod == VERTICAL_DISTANCE) {
        result += String("id_vertical_distance") + "=" + "checked;";
      }
    } else
    if (argName.equals("id_apActive")) {
      if (ourConfig.apIsActive == true) {
        result += argName + "=" + "checked;";
      }
    }
  }
  Serial.println();
  // strip last semicolon
  result.remove(result.length()-1);
  // result += String(";rudderDepth=") + ourRudderDepth;

  Serial.print("result:");
  Serial.println(result);
  server.send(200, "text/plane", result.c_str()); //Send the result value only to client ajax request
}

void handleWebRequests(){
  // if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";

  message += "client : ";
  message += server.client().remoteIP().toString();
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void checkHTMLArguments() {
  Serial.println("checkHTMLArguments()");
  String name = server.arg("name");
  if (name.length() != 0) {
    setDataReq();
  }
}

// =================================
// helper function
// =================================

double irr_low_pass_filter(double aSmoothedValue, double aCurrentValue, double aSmoothingFactor) {
  // IIR Low Pass Filter
  // y[i] := α * x[i] + (1-α) * y[i-1]
  //      := α * x[i] + (1 * y[i-1]) - (α * y[i-1])
  //      := α * x[i] +  y[i-1] - α * y[i-1]
  //      := α * ( x[i] - y[i-1]) + y[i-1]
  //      := y[i-1] + α * (x[i] - y[i-1])
  // mit α = 1- β
  //      := y[i-1] + (1-ß) * (x[i] - y[i-1])
  //      := y[i-1] + 1 * (x[i] - y[i-1]) - ß * (x[i] - y[i-1])
  //      := y[i-1] + x[i] - y[i-1] - ß * x[i] + ß * y[i-1]
  //      := x[i] - ß * x[i] + ß * y[i-1]
  //      := x[i] + ß * y[i-1] - ß * x[i]
  //      := x[i] + ß * (y[i-1] - x[i])
  // see: https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter
  return aCurrentValue + aSmoothingFactor * (aSmoothedValue - aCurrentValue);
}

float roundToDot5(double aValue) {
  return round(aValue * 2)/2;
}

void printMPU5060Offsets() {
   Serial.print("X Accel Offset: ");
   Serial.print(mpu.getXAccelOffset());
   Serial.print(", Y Accel Offset: ");
   Serial.print(mpu.getYAccelOffset());
   Serial.print(", Z Accel Offset: ");
   Serial.print(mpu.getZAccelOffset());
   Serial.println();
}

boolean isI2C_MMA8451Addr() {
  boolean retVal = false;
  if (ourI2CAddr == MMA8451ADDR1 || ourI2CAddr == MMA8451ADDR2 ) {
    retVal = true;
  }
  return retVal;
}

boolean isI2C_MPU6050Addr() {
  boolean retVal = false;
  if (ourI2CAddr == MPU6050ADDR1 || ourI2CAddr == MPU6050ADDR2 ) {
    retVal = true;
  }
  return retVal;
}

void initMMA8451() {
  Serial.println("initializing MMA8451 device");
  mma = Adafruit_MMA8451();

  Serial.println("Testing device connections...");
  if (mma.begin(ourSDA_Pin, ourSCL_Pin, ourI2CAddr)) {
    Serial.print("MMA8451 connection successful at : 0x");
    Serial.println(ourI2CAddr, HEX);
  } else {
    Serial.println("MMA8451 connection failed. MMA8452 assumed");
    ourSensorTypeName="MMA-8452";
  }
  mma.setRange(MMA8451_RANGE_2_G);
}

void initMPU5060() {
  Serial.println("Initializing MPU6050 device");
  Serial.println("Testing device connections...");
  mpu = MPU6050(ourI2CAddr);
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.print("MPU6050 connection successful at : 0x");
    Serial.println(ourI2CAddr, HEX);
  } else {
    Serial.println("MPU6050 connection failed");
  }
  if (isSensorCalibrated()) {
    // set stored calibration data
    mpu.setXAccelOffset(ourConfig.xAccelOffset);
    mpu.setYAccelOffset(ourConfig.yAccelOffset);
    mpu.setZAccelOffset(ourConfig.zAccelOffset);
    Serial.println("MPU6050 ist kalibriert mit folgenden Werten: ");
    printMPU5060Offsets();
  } else {
   Serial.println("MPU6050 ist nicht kalibriert !!!!");
  }
}

void triggerRestart() {
  // the restart has to be delayed to avoid, that the response to the restart request is not
  // answerd properly to the http-client. If there is no response the client, will resend the
  // restart request ;-(
  ourTriggerRestart = millis();
}

void restartESP() {
  if (ourTriggerRestart != 0) {
    unsigned long delay = millis() - ourTriggerRestart;
     // wait for 200ms with the restart
    if (delay > 200) {
      ourTriggerRestart = 0;
      ESP.restart();
    }
  }
}
void triggerCalibrateMPU6050() {
  ourTriggerCalibrateMPU6050 = true;
}

boolean isSensorCalibrated() {
  return (ourConfig.xAccelOffset != 0 || ourConfig.yAccelOffset != 0 || ourConfig.zAccelOffset != 0);
}

void calibrateMPU6050() {
  if (ourTriggerCalibrateMPU6050 == true) {
    printMPU5060Offsets();
    mpu.CalibrateAccel(15);
    mpu.CalibrateGyro(15);
    Serial.println("\nat 1500 Readings");
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    Serial.println("\nat 600 Readings");
    mpu.PrintActiveOffsets();
    Serial.println();
    mpu.CalibrateAccel(1);
    mpu.CalibrateGyro(1);
    Serial.println("700 Total Readings");
    mpu.PrintActiveOffsets();
    Serial.println();
    mpu.CalibrateAccel(1);
    mpu.CalibrateGyro(1);
    Serial.println("800 Total Readings");
    mpu.PrintActiveOffsets();
    Serial.println();
    mpu.CalibrateAccel(1);
    mpu.CalibrateGyro(1);
    Serial.println("900 Total Readings");
    mpu.PrintActiveOffsets();
    Serial.println();
    mpu.CalibrateAccel(1);
    mpu.CalibrateGyro(1);
    Serial.println("1000 Total Readings");
    mpu.PrintActiveOffsets();
    printMPU5060Offsets();
    ourConfig.xAccelOffset = mpu.getXAccelOffset();
    ourConfig.yAccelOffset = mpu.getYAccelOffset();
    ourConfig.zAccelOffset = mpu.getZAccelOffset();
    ourTriggerCalibrateMPU6050 = false;
  }
}

void doAsync() {
  calibrateMPU6050();
  restartESP();
}

void detectSensor() {

  // supported I2C HW connections schemas
  #define I2C_CONNECTIONS_SIZE 2
  uint8_t cableConnections[2][2] = {
   {D3, D4} ,   /* SDA, SCL */
   {D2, D1}    /* SDA, SCL */
  };

  // supported I2C devices / addresses
  #define I2C_ADDR_SIZE 4
  uint8_t I2CAddresses[I2C_ADDR_SIZE] = {
    MPU6050ADDR1, MPU6050ADDR2, MMA8451ADDR1, MMA8451ADDR2,
  };


  for (int i = 0; i < I2C_CONNECTIONS_SIZE; i++) {
    ourSDA_Pin = cableConnections[i][0];
    ourSCL_Pin = cableConnections[i][1];
    Wire.begin(ourSDA_Pin, ourSCL_Pin);
    for (int j = 0; j<I2C_ADDR_SIZE; j++) {
      ourI2CAddr = I2CAddresses[j];
      Wire.beginTransmission(ourI2CAddr);
      byte result = Wire.endTransmission();
      if (result == 0){
        if (isI2C_MPU6050Addr()) {
          ourSensorTypeName = "MPU-6050/GY521";
        }
        if (isI2C_MMA8451Addr()) {
          ourSensorTypeName = "MMA-8451";
        }
        Serial.print("Sensor [");
        Serial.print(ourSensorTypeName);
        Serial.print("] found at I2C pins ");
        Serial.print(ourSDA_Pin);
        Serial.print("/");
        Serial.print(ourSCL_Pin);
        Serial.print(" (SDA/SCL) at address 0x");
        Serial.print(ourI2CAddr, HEX);
        Serial.println();
        return;
      }
    }
  }
}

// =================================
// WiFi functions
// =================================

void setupWiFi() {
  // first try to connect to the stored WLAN, if this does not work try to
  // start as Access Point

  if (String(ourConfig.wlanSsid).length() != 0 ) {
    WiFi.mode(WIFI_STA) ; // client mode only
    WiFi.begin(ourConfig.wlanSsid, ourConfig.wlanPasswd);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ourConfig.wlanSsid);
    int connectCnt = 0;
    while (WiFi.status() != WL_CONNECTED && connectCnt++ < 20) {
      delay(500);
      Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("success!");
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("cannot connect to SSID :");
    Serial.println(ourConfig.wlanSsid);
    WiFi.mode(WIFI_AP) ; // client mode only
  }
  if (ourConfig.apIsActive) {
    Serial.print("Starting WiFi Access Point with  SSID: ");
    Serial.println(ap_ssid);
    //ESP32 As access point IP: 192.168.4.1
    // WiFi.mode(WIFI_AP) ; //Access Point mode
    boolean res = WiFi.softAP(ap_ssid, ourConfig.apPasswd);    //Password length minimum 8 char
    if(res ==true) {
      IPAddress myIP = WiFi.softAPIP();
      Serial.println("AP setup done!");
      Serial.print("Host IP Address: ");
      Serial.println(myIP);
      Serial.print("Please connect to SSID: ");
      Serial.print(ap_ssid);
      Serial.print(", PW: ");
      Serial.print(ourConfig.apPasswd);
      Serial.println(", Address: http://192.168.4.1");
    }
  }
}

// =================================
// OLED functions
// =================================

#ifdef SUPPORT_OLED
void setOLEDData() {
  double angle = getAngle();
  float outAngle = roundToDot5(angle);
  float outAmplitude = roundToDot5(getAmplitude(angle)));
  u8g2.firstPage();                                                 // Display values
  do {
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_t0_14_tf);
    u8g2.setCursor(1, 15);
    u8g2.print("Winkel:");
    u8g2.setFont(u8g2_font_crox4tb_tn);
    u8g2.setCursor(78, 15);
    u8g2.print(outAngle);
    u8g2.setFont(u8g2_font_t0_14_tf);
    u8g2.setCursor(1, 35);
    u8g2.print("Rudertiefe");
    u8g2.setFont(u8g2_font_crox4tb_tn);
    u8g2.setCursor(78, 35);
    u8g2.print(ourRudderDepth);
    u8g2.setFont(u8g2_font_t0_14_tf);
    u8g2.setCursor(1, 55);
    u8g2.print("Ruderweg");
    u8g2.setFont(u8g2_font_crox4tb_tn);
    u8g2.setCursor(78, 55);
    u8g2.print(outAmplitude);
  } while (u8g2.nextPage() );
}
#endif

// =================================
// EEPROM functions
// =================================

void showConfig(const char* aContext) {
  Serial.println(aContext);
  Serial.print("cfg version         = "); Serial.println(ourConfig.version);
  Serial.print("axis                = "); Serial.println(ourConfig.axis);
  Serial.print("amplitudePrecision  = "); Serial.println(ourConfig.amplitudePrecision);
  Serial.print("anglePrecision      = "); Serial.println(ourConfig.anglePrecision);
  Serial.print("apIsActive          = "); Serial.println(ourConfig.apIsActive);
  Serial.print("angleInversion      = "); Serial.println(ourConfig.angleInversion);
  Serial.print("amplitudeInversion  = "); Serial.println(ourConfig.amplitudeInversion);
  Serial.print("wlanSsid            = "); Serial.println(ourConfig.wlanSsid);
  Serial.print("wlanPasswd          = "); Serial.println(ourConfig.wlanPasswd);
  Serial.print("apPasswd            = "); Serial.println(ourConfig.apPasswd);
  Serial.print("xAccelOffet         = "); Serial.println(ourConfig.xAccelOffset);
  Serial.print("yAccelOffet         = "); Serial.println(ourConfig.yAccelOffset);
  Serial.print("zAccelOffet         = "); Serial.println(ourConfig.zAccelOffset);
  Serial.print("amplitudeCalcMethod = "); Serial.println(ourConfig.amplitudeCalcMethod);
}

void setDefaultConfig() {
  Serial.println("setDefaultConfig()");
  // Reset EEPROM bytes to '0' for the length of the data structure
  showConfig("setDefaultConfig() - old data:");
  ourConfig.axis = xAxis;
  strncpy(ourConfig.version , CONFIG_VERSION, CONFIG_VERSION_L);
  ourConfig.axis = xAxis;
  ourConfig.amplitudeInversion = 1;
  ourConfig.angleInversion = 1;
  ourConfig.apIsActive=true;
  ourConfig.anglePrecision = P010;
  ourConfig.amplitudePrecision = P050;
  strncpy(ourConfig.wlanSsid , "", CONFIG_SSID_L);
  strncpy(ourConfig.wlanPasswd, "", CONFIG_PASSW_L);
  strncpy(ourConfig.apPasswd, "12345678", CONFIG_PASSW_L);
  ourConfig.xAccelOffset = 0;
  ourConfig.yAccelOffset = 0;
  ourConfig.zAccelOffset = 0;
  ourConfig.amplitudeCalcMethod = ARC;
  saveConfig();
}


void saveConfig() {
  Serial.println("saveConfig()");
  showConfig("saveConfig - start");
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  EEPROM.put(0, ourConfig );
  delay(10);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
  showConfig("saveConfig - end");
}

void loadConfig() {
  Serial.println("loadConfig()");
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get(0, ourConfig );
  EEPROM.end();
  // config was never written to EEPROM, so set the default config data and save it to EEPROM
  if ( String(CONFIG_VERSION) != ourConfig.version ) {
    setDefaultConfig();
  }
  if (ourConfig.amplitudeCalcMethod < 0) {
    ourConfig.amplitudeCalcMethod = ARC;
  }
}

void checkHWReset(uint8_t aPin) {
  // pull the given pin
  Serial.print("checking HW reset pin ");
  Serial.print(aPin);
  Serial.println(" for force HW config reset");
  pinMode(aPin, INPUT_PULLUP);
  delay(100);
  uint8_t cnt=0;
  for (int i=0; i<10; i++) {
    delay(20);
    if (digitalRead(aPin) == LOW) {
      cnt++;
    }
  }
  if (cnt == 10) {
    Serial.print("configuration reset by HW pin to GRD !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ");
    // Serial.println(cnt);
    setDefaultConfig();
  }
}
