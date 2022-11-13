#include "handlers.hpp"
#include "Arduino.h"
#include <can_format.hpp>
#include <CAN.h>

void noneHandler(Parser p)
{
    Serial.println("Invalid command. Use \"help\" for more information.");
}


void helpHandler(Parser p)
{
    char* helpMsg[] = {
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

void setOpModeHandler(Parser p)
{
    OPMODE_M packet;
    Srf02Config::OperationMode opMode = p.operationMode();

    packet.sensorId = p.sensorId();
    packet.opMode = opMode;

    if( opMode == Srf02Config::OperationMode::on_period )
        packet.period_ms = p.period();

    uint8_t buffer[sizeof(packet)];
    memcpy(buffer, &packet, sizeof(packet));

    // TODO: Prevent sending period_ms when not using onPeriod operation mode
    CAN.beginPacket(CAN_ID::OPMODE);
    CAN.write(buffer, sizeof(packet));
    CAN.endPacket();
}

// void listSensorsHandler(Parser p);
// void getStatusHandler(Parser p);
// void setDelayHandler(Parser p);
// void setUnitHandler(Parser p);