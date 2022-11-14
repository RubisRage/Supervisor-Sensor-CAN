#include "message_handlers.hpp"
#include <can_format.hpp>
#include <CAN.h>
#include <srf02_config.hpp>
#include "Arduino.h"

SENSOR_MEASUREMENT_M measurementPacket;
bool measurementPacketReady = false;

static void sendError(uint8_t code)
{
    Serial.println("Error");
}

static void sendAck()
{
    Serial.println("Ack");
    CAN.beginPacket(CAN_ID::ACK);
    CAN.endPacket();
}

static void registerMeasurement(const Srf02& sensor, OPMODE_M srcPacket, uint16_t range){
    SENSOR_MEASUREMENT_M packet;

    packet.sensorId = srcPacket.sensorId;
    packet.unit = sensor.unit();
    packet.range = range;

    measurementPacket = packet;
    measurementPacketReady = true;
}

template<typename T>
static void readMessage(T& packet)
{
    uint8_t buffer[sizeof(T)];
    int current;

    for(size_t i = 0; (current = CAN.read()) != -1; i++)
    {
        buffer[i] = static_cast<uint8_t>(current);
    }

    memcpy(&packet, buffer, sizeof(T));
}

void opModeHandler(Srf02* sensors, size_t sensorCount)
{
    OPMODE_M packet; 
    readMessage(packet);

    // TODO: Define error codes and implement send error func
    if(packet.sensorId >= sensorCount)
        sendError(0);

    Srf02::Status ret;
    Srf02 sensor = sensors[packet.sensorId];

    switch(packet.opMode)
    {
        case Srf02Config::OperationMode::off:
        {
            Serial.println("Off");
            sensors[packet.sensorId].off();
            sendAck();
            break;
        }
        case Srf02Config::OperationMode::on_period:
        {
            auto callback = [](uint16_t range){

                // registerMeasurement(sensor, packet, range);
            };

            ret = sensor.onPeriod(packet.period_ms, callback);

            if(ret == Srf02::Status::ok)
                sendAck();
            else 
                sendError(ret);

            break;
        }
        case Srf02Config::OperationMode::one_shot:
        {
            uint16_t range;

            ret = sensors[packet.sensorId].oneShot(range);

            if(ret == Srf02::Status::ok)
                registerMeasurement(sensor, packet, range);
            else 
                sendError(ret);

            break;
        }
    }
}

void unitHandler(Srf02* sensors, size_t sensorCount)
{
    UNIT_M packet; 
    readMessage(packet);

    if (packet.sensorId >= sensorCount)
        sendError(0);

    sensors[packet.sensorId].unit(packet.unit);
}

void delayHandler(Srf02* sensors, size_t sensorCount)
{
    DELAY_M packet;
    readMessage(packet);

    if (packet.sensorId >= sensorCount)
        sendError(0);

    sensors[packet.sensorId].delay(packet.delay_ms);
}

void statusHandler(Srf02*, size_t);

void listHandler(Srf02* sensors, size_t sensorCount)
{
    LIST_SENSORS_RESPONSE_M packet;

    packet.sensorId1 = 0;
    packet.sensorId1 = 1;

    CAN.beginPacket(CAN_ID::LIST_RESPONSE);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

// void Handler(Srf02*, size_t);