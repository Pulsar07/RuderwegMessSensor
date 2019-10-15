#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#include "RuderwegMessSensorTypes.h"
#include "htmlRootPage.h"
#include "htmlAdminPage.h"
#include "htmlScript.h"
#include "htmlCSS.h"

/*
  !! create a file "myWifiSettings" with the following content:
      // select on of these supported sensors connceted via I2C to D1 (SCL), D2 (SDA)
      // #define SUPPORT_MMA8451
      // #define SUPPORT_MPU6050

      // a additional local connected for OLED display
      // #define SUPPORT_OLED
*/
#include "mySettings.h"

#ifdef SUPPORT_MMA8451
#include <Adafruit_MMA8451.h>         // MMA8451 library
#endif

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
//         support flight phase supporting measure with null, min, max values
//         CSS based layout added
#define WM_VERSION "V0.23"

/**
 * \file winkelmesser.ino
 *
 * \brief small tool to measure the amplitudes of model glider rudders
 *
 * \author Author: Rainer Stransky
 *
 * \copyright This project is released under the GNU Public License v3
 *          see https://www.gnu.org/licenses/gpl.html.
 * Contact: opensource@so-fa.de
 *
 */

#ifdef SUPPORT_MPU6050
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
#endif
#ifdef SUPPORT_MMA8451
Adafruit_MMA8451 mma = Adafruit_MMA8451();
#endif


#ifdef SUPPORT_OLED
#include <U8g2lib.h>         // OLED library  https://github.com/olikraus/u8g2
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D1, /* data=*/ D2); //OLED am Wemos D1 mini Pin 1 und 2
#endif

static configData_t ourConfig;
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

  #ifdef SUPPORT_MPU6050
     // Wire.begin();
     Wire.begin(0, 2); //SDA, SCL
     Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
     Wire.write(0x6B); // PWR_MGMT_1 register
     Wire.write(0); // set to zero (wakes up the MPU-6050)
     Wire.endTransmission(true);
  #endif
  #ifdef SUPPORT_MMA8451
    mma.begin();
    mma.setRange(MMA8451_RANGE_2_G);
  #endif

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

  #ifdef SUPPORT_MPU6050
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers

    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    ourAccelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    ourAccelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    ourAccelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  #endif
  #ifdef SUPPORT_MMA8451
    mma.read();
    ourAccelerometer_x = mma.x;
    ourAccelerometer_y = mma.y;
    ourAccelerometer_z = mma.z;
  #endif

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
  static double ourAngle = 0;
  switch (ourConfig.axis) {
    case xAxis:
      ourAngle = ourSmoothedAngle_x;
    break;
    case yAxis:
      ourAngle = ourSmoothedAngle_y;
    break;
    case zAxis:
      ourAngle = ourSmoothedAngle_z;
    break;
  }
  return (ourAngle - ourTara) * ourConfig.angleInversion;
}

double getAmplitude(double aAngle) {
  return (aAngle/360*M_PI * 2 * ourRudderDepth) * ourConfig.amplitudeInversion;
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
      ourConfig.angleInversion = -1;
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
      Serial.println("not setting AP password, too short : " + String(ourConfig.apPasswd));
    }
  } else
  if (name == "cmd_saveConfig") {
     showConfig("==== before");
     saveConfig();
     showConfig("==== after");
  } else
  if (name == "cmd_resetConfig") {
     Serial.println("before reset");
     setDefautConfig();
     showConfig("==== after");
  }

  if (sendResponse) {
    Serial.println("send response to server");
    server.send(200, "text/plane", retVal); // send an valid answer
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
        result += String("id_ruddernull") + "=" + String(ourNullAmpl) + ";";
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
    if (argName.equals("id_wlanSsid")) {
      if (String(ourConfig.wlanSsid).length() != 0) {
        result += argName + "=" + ourConfig.wlanSsid + ";";
      }
    } else
    if (argName.equals("id_wlanPasswd")) {
      if (String(ourConfig.wlanPasswd).length() != 0) {
        result += argName + "=" + "************;";
      }
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
    Serial.print("cannot connect to SSID ");
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

void showConfig(char* aContext) {
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
}

void setDefautConfig() {
  Serial.println("setDefaultConfig()");
  // Reset EEPROM bytes to '0' for the length of the data structure
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
  saveConfig();
}


void saveConfig() {
  Serial.println("saveConfig()");
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  EEPROM.put(0, ourConfig );
  delay(10);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
}

void loadConfig() {
  Serial.println("loadConfig()");
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get(0, ourConfig );
  EEPROM.end();
  // config was never written to EEPROM, so set the default config data and save it to EEPROM
  if ( String(CONFIG_VERSION) != ourConfig.version ) {
    setDefautConfig();
  }
}

void checkHWReset(uint8_t aPin) {
  // pull the given pin
  Serial.print("checking HW reset pin ");
    Serial.print(aPin);
    Serial.println(" for force HW config reset");
  pinMode(aPin, INPUT);
  delay(100);
  if (digitalRead(aPin) == LOW) {
    Serial.println("HW reset detected.");
    // setDefautConfig();
  }

}
