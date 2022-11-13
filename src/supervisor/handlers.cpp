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
    // TODO: Print help
}

void setOpModeHandler(Parser p)
{
    OPMODE_M packet;
    Srf02Config::OperationMode opMode = p.operationMode();

    packet.opMode = opMode;

    if( opMode == Srf02Config::OperationMode::on_period )
        packet.period_ms = p.period();

    uint8_t buffer[sizeof(packet)];
    memcpy(buffer, &packet, sizeof(packet));

    CAN.beginPacket(CAN_ID::OPMODE);
    CAN.write(buffer, sizeof(packet));
    CAN.endPacket();
}

// void listSensorsHandler(Parser p);
// void getStatusHandler(Parser p);
// void setDelayHandler(Parser p);
// void setUnitHandler(Parser p);