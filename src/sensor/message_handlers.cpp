#include "message_handlers.hpp"
#include <can_format.hpp>
#include <CAN.h>
#include <srf02_config.hpp>
#include "Arduino.h"
#include <globals.hpp>

static void registerMeasurement(void* arg){
    int sensorId = (int) arg;
    Srf02 sensor = sensors[sensorId];

    measurementPacket.sensorId = sensor.sensorId();
    measurementPacket.unit = sensor.unit();

    measurementPacketReady = true;
}

void opModeHandler()
{
    OPMODE_M packet; 
    readMessage(packet);

    if(packet.sensorId >= SENSOR_COUNT)
        sendError(CAN_ERR::unknown_sensor);

    Srf02::Status ret;
    Srf02 sensor = sensors[packet.sensorId];

    switch(packet.opMode)
    {
        case Srf02Config::OperationMode::off:
        {
            Serial.println("Off");
            sensor.off();
            sendAck();
        }
        break;
        case Srf02Config::OperationMode::on_period:
        {
            ret = sensor.onPeriod(packet.period_ms, registerMeasurement);

            if(ret == Srf02::Status::ok)
                sendAck();
            else 
                sendError(ret);

        }
        break;
        case Srf02Config::OperationMode::one_shot:
        {
            sendMeasurement(sensor);
        }
        break;
    }
}

void unitHandler()
{
    UNIT_M packet; 
    readMessage(packet);

    if (packet.sensorId >= SENSOR_COUNT)
        sendError(CAN_ERR::unknown_sensor);

    sensors[packet.sensorId].unit(packet.unit);
    sendAck();
}

void delayHandler()
{
    DELAY_M packet;
    readMessage(packet);

    if (packet.sensorId >= SENSOR_COUNT)
        sendError(CAN_ERR::unknown_sensor);

    Srf02::Status ret_ = sensors[packet.sensorId].delay(packet.delay_ms);

    if(ret_ == Srf02::Status::ok)
        sendAck();
    else 
        sendError(ret_);
}

void statusHandler()
{
    STATUS_M packet;
    readMessage(packet);

    if (packet.sensorId >= SENSOR_COUNT)
        sendError(CAN_ERR::unknown_sensor);

    Srf02 sensor = sensors[packet.sensorId];
    STATUS_RESPONSE_M response;
    
    Serial.print("Sensor address: ");
    Serial.println(sensor.address(), HEX);
    
    response.sensorId = sensor.sensorId();
    response.i2cAddr = sensor.address();
    response.delay = sensor.delay();
    response.unit = sensor.unit();

    if(sensor.onPeriod())
    {
        response.period_ms = sensor.period();
    }
    else 
        response.period_ms = 0;


    CAN.beginPacket(CAN_ID::STATUS_RESPONSE);
    CAN.write((uint8_t*)&response, sizeof(response));
    CAN.endPacket();
}

void listHandler()
{
    LIST_SENSORS_RESPONSE_M packet;

    packet.sensorId1 = 0;
    packet.sensorId1 = 1;

    CAN.beginPacket(CAN_ID::LIST_RESPONSE);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}