#include "message_handlers.hpp"
#include <can_format.hpp>
#include <CAN.h>
#include <srf02_config.hpp>
#include "Arduino.h"
#include <globals.hpp>

static void registerMeasurement(void* arg){
    Srf02* sensor = (Srf02*) arg;

    measurementPacket.sensorId = sensor->sensorId();
    measurementPacket.unit = sensor->unit();

    measurementPacketReady = true;
}

void opModeHandler()
{
    OPMODE_M packet; 
    readMessage(packet);

    // TODO: Define error codes and implement send error func
    if(packet.sensorId >= SENSOR_COUNT)
        sendError(0);

    Srf02::Status ret;
    Srf02 sensor = sensors[packet.sensorId];

    switch(packet.opMode)
    {
        case Srf02Config::OperationMode::off:
        {
            Serial.println("Off");
            sensor.off();
            sendAck();
            break;
        }
        case Srf02Config::OperationMode::on_period:
        {
            ret = sensor.onPeriod(packet.period_ms, registerMeasurement);

            if(ret == Srf02::Status::ok)
                sendAck();
            else 
                sendError(ret);

            break;
        }
        case Srf02Config::OperationMode::one_shot:
        {
            if(ret == Srf02::Status::ok)
                registerMeasurement(&sensor);
            else 
                sendError(ret);

            break;
        }
    }
}

void unitHandler()
{
    UNIT_M packet; 
    readMessage(packet);

    if (packet.sensorId >= SENSOR_COUNT)
        sendError(0);

    sensors[packet.sensorId].unit(packet.unit);
}

void delayHandler()
{
    DELAY_M packet;
    readMessage(packet);

    if (packet.sensorId >= SENSOR_COUNT)
        sendError(0);

    sensors[packet.sensorId].delay(packet.delay_ms);
}

void statusHandler();

void listHandler()
{
    LIST_SENSORS_RESPONSE_M packet;

    packet.sensorId1 = 0;
    packet.sensorId1 = 1;

    CAN.beginPacket(CAN_ID::LIST_RESPONSE);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

// void Handler(Srf02*, size_t);