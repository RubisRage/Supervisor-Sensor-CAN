#include <srf02.hpp>
#include <Arduino.h>
#include <CAN.h>
#include <can_format.hpp>
#include "message_handlers.hpp"

#define SENSOR_COUNT 2

Srf02 sensors[SENSOR_COUNT] = {Srf02(0xE6), Srf02(0xE8)};

message_handler_t messageHandlers[MESSAGE_HANDLER_NUM];

void setup()
{
  Srf02::begin();
  Serial.begin(9600);

  while(!Serial);

  if(!CAN.begin(500E3))
  {
    Serial.println("Stating CAN failed!");
    exit(1);
  }

  messageHandlers[CAN_ID::OPMODE - CAN_ID_OFFSET] = opModeHandler;
}

void loop()
{
  int packetSize = CAN.parsePacket();

  // Serial.println("Looping...");

  if(packetSize)
  {
    long id = CAN.packetId();
    Serial.print("LLega mensaje ->");
    Serial.println(id);

    messageHandlers[id - CAN_ID_OFFSET](sensors, SENSOR_COUNT);
  }
}