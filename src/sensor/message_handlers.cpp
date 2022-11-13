#include "message_handlers.hpp"
#include <can_format.hpp>
#include <CAN.h>
#include <srf02_config.hpp>

static void sendError(uint8_t code)
{

}

static void sendAck()
{

}

static void sendMeasurement(OPMODE_M srcPacket, uint16_t range){
    SENSOR_MEASUREMENT_M packet;
    uint8_t buffer[sizeof(packet)];
}

void opModeHandler(Srf02* sensors, size_t sensorCount)
{
    uint8_t buffer[sizeof(OPMODE_M)];
    uint8_t current;

    for(size_t i = 0; (current = CAN.read()) != -1; i++)
        buffer[i] = current;

    OPMODE_M packet;

    memcpy(&packet, buffer, sizeof(OPMODE_M));

    // TODO: Define error codes and implement send error func
    if(packet.sensorId > sensorCount)
        sendError(0);

    switch(packet.opMode)
    {
        case Srf02Config::OperationMode::off:

            sensors[packet.sensorId].off();
            sendAck();
            break;

        case Srf02Config::OperationMode::on_period:

            auto callback = [](uint16_t range){
                sendMeasurement(packet, range);
            }

            Srf02::Status ret = sensors[packet.sensorId].onPeriod(packet.period_ms, sendMeasurement);

            if(ret == Srf02::Status::ok)
                sendAck();
            else 
                sendError(ret);

            break;

        case Srf02Config::OperationMode::one_shot:
            uint16_t range;

            Srf02::Status ret = sensors[packet.sensorId].oneShot(range);

            if(ret == Srf02::Status::ok)
                sendMeasurement(range);
            else 
                sendError(ret);

            break;
    }
}

void unitHandler(Srf02*, size_t);
void delayHandler(Srf02*, size_t);
void statusHandler(Srf02*, size_t);
void listHandler(Srf02*, size_t);
void Handler(Srf02*, size_t);