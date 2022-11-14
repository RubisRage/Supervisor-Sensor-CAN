#pragma once

#include <cstdlib>
#include <srf02.hpp>
#include <can_format.hpp>

extern SENSOR_MEASUREMENT_M measurementPacket;
extern bool measurementPacketReady;

#define MESSAGE_HANDLER_NUM 6

typedef void (*message_handler_t) (Srf02*, size_t);

void opModeHandler(Srf02*, size_t);
void unitHandler(Srf02*, size_t);
void delayHandler(Srf02*, size_t);
void statusHandler(Srf02*, size_t);
void listHandler(Srf02*, size_t);
// void Handler(Srf02*, size_t);