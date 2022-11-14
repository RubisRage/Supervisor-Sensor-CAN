#include <srf02.hpp>
#include <can_format.hpp>

#define SENSOR_COUNT 2

extern Srf02 sensors[SENSOR_COUNT];
extern bool measurementPacketReady;
extern SENSOR_MEASUREMENT_M measurementPacket;

 