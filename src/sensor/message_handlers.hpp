#pragma once

#include <can_format.hpp>
#include <srf02.hpp>

extern SENSOR_MEASUREMENT_M measurementPacket;
extern bool measurementPacketReady;

#define MESSAGE_HANDLER_NUM 10

typedef void (*message_handler_t) ();

void opModeHandler();
void unitHandler();
void delayHandler();
void statusHandler();
void listHandler();