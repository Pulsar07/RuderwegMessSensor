#ifndef RuderwegMessSensorTypes_h
#define RuderwegMessSensorTypes_h

#include <WString.h>

typedef enum {
  xAxis,
  yAxis,
  zAxis,
} referenceAxis_t;

typedef enum {
  P001,
  P010,
  P050,
  P100,
} precision_t;

typedef enum {
  ARC,
  CHORD,
  VERTICAL_DISTANCE,
} amplitude_calc_method_t;

#define CONFIG_VERSION "RSV1"
#define CONFIG_VERSION_L 5
#define CONFIG_SSID_L 16
#define CONFIG_PASSW_L 64
// Types 'byte' und 'word' doesn't work!
typedef struct {
  char version[CONFIG_VERSION_L];
  char wlanSsid[CONFIG_SSID_L];
  char wlanPasswd[CONFIG_PASSW_L];
  char apSsid[CONFIG_SSID_L];
  char apPasswd[CONFIG_PASSW_L];
  boolean apIsActive;
  referenceAxis_t axis;
  int angleInversion;
  precision_t anglePrecision;
  precision_t amplitudePrecision;
  int amplitudeInversion;
  amplitude_calc_method_t amplitudeCalcMethod;
  int16_t xAccelOffset;
  int16_t yAccelOffset;
  int16_t zAccelOffset;
  int16_t xGyroOffset;
  int16_t yGyroOffset;
  int16_t zGyroOffset;
  int16_t calibrationOffsetHigh;
  int16_t calibrationOffsetLow;
  boolean calibrationOffsetEnabled;
} configData_t;





#endif
