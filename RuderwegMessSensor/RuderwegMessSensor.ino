
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Wire.h" // This library allows you to communicate with I2C devices.
#include "html_page.h"
// !! create a file "myWifiSettings" with the following content:
//   #define MY_WIFI_SSID "<SSID>"
//   #define MY_WIFI_PASSWORD "<CREDENTIALS>"
#include "myWifiSettings.h"
#define WM_VERSION "V0.13"

// #define SUPPORT_MMA8451
#define SUPPORT_MPU6050

#ifdef SUPPORT_MMA8451
#include <Adafruit_MMA8451.h>         // MMA8451 library
#endif

// Version history
// V0.10 : full functional initial version
// V0.11 : use wifi data from private header file
// V0.12 : support for MMA8451 added (no test HW available)
<<<<<<< HEAD:RuderwegMessSensor.ino
// V0.13 : displayed angle with more precision
=======
// V0.13 : support for OLED
>>>>>>> 4e581ff8438e2f2bf23d8182ba22de026b4352af:RuderwegMessSensor/RuderwegMessSensor.ino

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

//Jochen
#define SUPPORT_OLED

#ifdef SUPPORT_OLED
#include <U8g2lib.h>         // OLED library  https://github.com/olikraus/u8g2
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D1, /* data=*/ D2); //OLED am Wemos D1 mini Pin 1 und 2
#endif
//Jochen

int16_t ourAccelerometer_x, ourAccelerometer_y, ourAccelerometer_z; // variables for ourAccelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
char tmp_str[7]; // temporary variable used in convert function
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

// WiFi network settings
const char* ssid = MY_WIFI_SSID;
const char* password = MY_WIFI_PASSWORD;

ESP8266WebServer server(80);    // Server Port  hier einstellen

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
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
  ourTaraAngle_x = ourSmoothedAngle_x;
  ourTaraAngle_y = ourSmoothedAngle_y;
  ourTaraAngle_z = ourSmoothedAngle_z;
  ourTaraGyro_x = ourSmoothedGyro_x;
  ourTaraGyro_y = ourSmoothedGyro_y;
  ourTaraGyro_z = ourSmoothedGyro_z;
  Serial.println("tara angle set to :" + String(ourTaraAngle_z));
  server.send(200, "text/plane", ""); //Send web page
}

void handleDataReq() {
  double resultingAngle = ourSmoothedAngle_y - ourTaraAngle_y;
  double smoothedAmplitude = resultingAngle/360*M_PI * 2 * ourRudderDepth;

  String angleString = String((float) resultingAngle);
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

  #ifdef SUPPORT_MPU6050
     Wire.begin();
     Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
     Wire.write(0x6B); // PWR_MGMT_1 register
     Wire.write(0); // set to zero (wakes up the MPU-6050)
     Wire.endTransmission(true);
  #endif
  #ifdef SUPPORT_MMA8451
    mma.begin();
    mma.setRange(MMA8451_RANGE_2_G);
  #endif
  
  //Jochen
#ifdef SUPPORT_OLED
u8g2.begin();
#endif
//Jochen

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
  ourSmoothedAngle_x = irr_low_pass_filter(ourSmoothedAngle_x,
    atan2(ourAccelerometer_y, ourAccelerometer_z) * 180 / M_PI, smooth);
  ourSmoothedAngle_y = irr_low_pass_filter(ourSmoothedAngle_y,
    atan2(ourAccelerometer_x, ourAccelerometer_z) * 180 / M_PI, smooth);
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
  Serial.print(String("WX = ") + roundToDot5(effAngle_x));
  Serial.print(String(" WY = ") + roundToDot5(effAngle_y));
  Serial.print(String(" WZ = ") + roundToDot5(effAngle_z));
  Serial.print(String(" GX = ") + roundToDot5(effGyro_x));
  Serial.print(String(" GY = ") + roundToDot5(effGyro_y));
  Serial.print(String(" GZ = ") + roundToDot5(effGyro_z));
  Serial.println();
<<<<<<< HEAD:RuderwegMessSensor.ino
=======
  /*
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  */
  
  //Jochen
#ifdef SUPPORT_OLED
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
     } while ( u8g2.nextPage() );
#endif
//Jochen  
>>>>>>> 4e581ff8438e2f2bf23d8182ba22de026b4352af:RuderwegMessSensor/RuderwegMessSensor.ino
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
