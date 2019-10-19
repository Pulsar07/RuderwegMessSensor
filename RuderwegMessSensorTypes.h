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

#define CONFIG_VERSION "RSV1"
#define CONFIG_VERSION_L 5
#define CONFIG_SSID_L 16
#define CONFIG_PASSW_L 64
// Types 'byte' und 'word' doesn't work!
typedef struct {
  char version[CONFIG_VERSION_L];
  referenceAxis_t axis;
  precision_t anglePrecision;
  precision_t amplitudePrecision;
  int amplitudeInversion;
  int angleInversion;
  char wlanSsid[CONFIG_SSID_L];
  char wlanPasswd[CONFIG_PASSW_L];
  char apPasswd[CONFIG_PASSW_L];
  boolean apIsActive;
  int16_t xAccelOffset;
  int16_t yAccelOffset;
  int16_t zAccelOffset;
} configData_t;





#endif
