
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#include "html_page.h"
// !! create a file "myWifiSettings" with the following content:
//   #define MY_WIFI_SSID "<SSID>"
//   #define MY_WIFI_PASSWORD "<CREDENTIALS>"
#include "myWifiSettings.h"
#define WM_VERSION "V0.11"

// Version history
// V0.10 : full functional initial version
// V0.11 : use wifi data from private header file

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

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
char tmp_str[7]; // temporary variable used in convert function
static double ourSmoothedAngle = 0;
static double ourTaraAngle = 0;
static double ourRudderDepth = 30;

// WiFi network settings
const char* ssid = MY_WIFI_SSID;
const char* password = MY_WIFI_PASSWORD;

ESP8266WebServer server(80);    // Server Port  hier einstellen

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

float roundToDot5(double aValue) {
  return -round(aValue * 2)/2;
}

void handleRudderDepthReq() {
  String depth = server.arg("value"); //Refer  xhttp.open("GET", "setRudderDepth?value="+aDepth", true);
  ourRudderDepth = double(atoi(depth.c_str()))/10;
  Serial.println("rudder depth is :" + String(ourRudderDepth));
  server.send(200, "text/plane", ""); //Send web page
}

void handleTaraAngleReq() {
  ourTaraAngle = ourSmoothedAngle;
  Serial.println("tara angle set to :" + String(ourTaraAngle));
  server.send(200, "text/plane", ""); //Send web page
}

void handleDataReq() {
  double resultingAngle = ourSmoothedAngle - ourTaraAngle;
  double smoothedAmplitude = resultingAngle/360*M_PI * 2 * ourRudderDepth;

  String angleString = String(roundToDot5(resultingAngle));
  String amplitudeString = String(roundToDot5(smoothedAmplitude));

  server.send(200, "text/plane", angleString + ";" + amplitudeString); //Send the result value only to client ajax request
}

void handleVersionReq() {
  server.send(200, "text/plane", WM_VERSION);
}

void handleWebRequests(){
  // if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
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

void setup()
{
  delay(1000);
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
  //  Bechandlung der Ereignissen anschlissen


  server.on("/",handleRoot);
  server.on("/readData",handleDataReq);
  server.on("/readVersion",handleVersionReq);
  server.on("/taraAngle",handleTaraAngleReq);
  server.on("/setRudderDepth",handleRudderDepthReq);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI

  server.begin();               // Starte den Server
  Serial.println("HTTP Server gestartet");
}


/*
void setup()
{
Serial.begin(115200);         // Serielle schnittstelle initialisieren
Serial.println("");           // Lehere Zeile ausgeben
Serial.println("Starte WLAN-Hotspot \"astral\"");
WiFi.mode(WIFI_AP);           // access point modus
WiFi.softAP("astral", "12345678");    // Name des Wi-Fi netzes
delay(500);                   //Abwarten 0,5s
Serial.print("IP Adresse ");  //Ausgabe aktueller IP des Servers
Serial.println(WiFi.softAPIP());

//  Bechandlung der Ereignissen anschlissen
server.on("/", Ereignis_Index);
server.on("/1.html", Ereignis_SchalteON);
server.on("/0.html", Ereignis_SchalteOFF);

server.begin();               // Starte den Server
Serial.println("HTTP Server gestartet");
}

*/



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

void readMotionSensor() {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  // print out data
  double currentAngle =  atan2(accelerometer_x, accelerometer_z) * 180 / M_PI;
  ourSmoothedAngle = irr_low_pass_filter(ourSmoothedAngle, currentAngle, 0.98d);
}

void prepareMotionData() {
  float outAngle = round(ourSmoothedAngle * 2)/2;
  Serial.print("Winkel = "); Serial.print(outAngle); Serial.println(" Grad");
  int rudderdepth = 30;
  double smoothedAmplitude = ourSmoothedAngle/360*M_PI * 2 * rudderdepth;
  float outAmplitude = round(smoothedAmplitude * 2)/2;
  Serial.print("Ruderweg = "); Serial.print(outAmplitude); Serial.println("mm");

  /*
  Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();
  */
}
void loop()
{
  static unsigned long last = 0;
  server.handleClient();

  readMotionSensor();
  if ( (millis() - last) > 1000) {
    prepareMotionData();
    last = millis();
  }
}
