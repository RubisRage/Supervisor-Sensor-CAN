#include <srf02.hpp>
#include <Arduino.h>
#include <CAN.h>
#include <can_format.hpp>
#include "message_handlers.hpp"
#include <globals.hpp>

Srf02 sensors[SENSOR_COUNT] = {Srf02(0xE6, 0), Srf02(0xE8, 1)};
bool measurementPacketReady = false;
SENSOR_MEASUREMENT_M measurementPacket;

message_handler_t messageHandlers[MESSAGE_HANDLER_NUM];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

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

  if(packetSize)
  {
    long id = CAN.packetId();
    Serial.print("LLega mensaje ->");
    Serial.println(id);

    messageHandlers[id - CAN_ID_OFFSET]();
  }

  if(measurementPacketReady)
  {
    uint16_t range;
    sensors[measurementPacket.sensorId].oneShot(range);
    measurementPacket.range = range;

    Serial.print("Range (");
    Serial.print(measurementPacket.sensorId);
    Serial.print("): ");
    Serial.println(range);

    CAN.beginPacket(CAN_ID::SENSOR_MEASUREMENT);
    CAN.write((uint8_t*)&measurementPacket, sizeof(measurementPacket));
    CAN.endPacket();

    measurementPacketReady = false;
  }
}