#include "handlers.hpp"
#include "Arduino.h"
#include <can_format.hpp>
#include <CAN.h>

void noneHandler(Parser& p)
{
    Serial.println("Invalid command. Use \"help\" for more information.");
}


void helpHandler(Parser& p)
{
    static char* helpMsg[] = {
        "Available commands and syntax:\n",
        "help: Display this message.\n",
        "us: List available sensors.\n",
        "us <id> status: Display the specified sensor's configuration.\n",
        "us <id> delay <ms>: Configure the minimum time (in ms) between measurements for the specified sensor.\n",
        "us <id> unit {inc | cm | ms}: Configure the unit in which measurements are taken for the specified sensor.\n",
        "us <id> {one-shot | on <period_ms> | off }: Configure the operation mode of the specified sensor.\n",
        "\tone-shot: Take one measurement. Disables \"on <period_ms>\" operation mode.\n",
        "\ton <period_ms>: Enable periodic measurments each <period_ms> milliseconds.\n",
        "\toff: Disables \"on <period_ms>\" operation mode.\n"
    };

    for(char* s : helpMsg)
        Serial.print(s);
}

void setOpModeHandler(Parser& p)
{
    OPMODE_M packet;
    Srf02Config::OperationMode opMode = p.operationMode();

    packet.sensorId = p.sensorId();
    packet.opMode = opMode;

    if( opMode == Srf02Config::OperationMode::on_period )
        packet.period_ms = p.period();

    CAN.beginPacket(CAN_ID::OPMODE);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

void listSensorsHandler(Parser& p)
{
    CAN.beginPacket(CAN_ID::LIST, 1, true);
    CAN.endPacket();
}

void getStatusHandler(Parser& p)
{
    STATUS_M packet;
    packet.sensorId = p.sensorId();

    CAN.beginPacket(CAN_ID::STATUS);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

void setDelayHandler(Parser& p)
{
    DELAY_M packet;

    packet.sensorId = p.sensorId();
    packet.delay_ms = p.delay();

    CAN.beginPacket(CAN_ID::DELAY);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();    
}

 void setUnitHandler(Parser& p)
{
    UNIT_M packet;
    Srf02Config::Unit unit = p.unit();

    packet.sensorId = p.sensorId();
    packet.unit = unit;

    CAN.beginPacket(CAN_ID::UNIT);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}