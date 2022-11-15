#include <can_format.hpp>
#include <CAN.h>

void sendError(uint8_t code)
{
    Serial.println("Error");
}

void sendAck()
{
    Serial.println("Ack");
    CAN.beginPacket(CAN_ID::ACK);
    CAN.endPacket();
}

void sendMeasurement(Srf02& sensor)
{
    SENSOR_MEASUREMENT_M packet;

    packet.sensorId = sensor.sensorId();
    packet.unit = sensor.unit();
    sensor.oneShot(packet.range);

    Serial.print("Range: ");
    Serial.println(packet.range);

    CAN.beginPacket(CAN_ID::SENSOR_MEASUREMENT);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

template<typename T>
void readMessage(T& packet)
{
    uint8_t buffer[sizeof(T)];
    int current;

    for(size_t i = 0; (current = CAN.read()) != -1; i++)
    {
        buffer[i] = static_cast<uint8_t>(current);
    }

    memcpy(&packet, buffer, sizeof(T));
}

/* readMessage function instantiation */
template void readMessage<OPMODE_M>(OPMODE_M&);
template void readMessage<UNIT_M>(UNIT_M&);
template void readMessage<DELAY_M>(DELAY_M&);