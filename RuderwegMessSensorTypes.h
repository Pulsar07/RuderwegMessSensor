#ifndef RuderwegMessSensorTypes_h
#define RuderwegMessSensorTypes_h

#include <WString.h>

typedef enum {
  xAxis,
  yAxis,
} referenceAxis_t;

// Types 'byte' und 'word' doesn't work!
typedef struct {
  referenceAxis_t axis;
  char SSID[31];                    // SSID of WiFi
  char password[128];                // Password of WiFi
} configData_t;




#endif
