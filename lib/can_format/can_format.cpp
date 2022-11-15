#include <can_format.hpp>
#include <CAN.h>

const char* const errorMsg[] = {
    "Tried taking measurement before specified delay time was accomplished.",
    "Tried setting delay time to a value lower than MINIMUM_DELAY_MS",
    "Tried setting period time to a value lower than the specified delay.",
    "Failed to initialize hardware timer.",
    "Connecting to the Srf02 sensor timed out."
};


void sendError(uint8_t code)
{
    ERR_M packet;
    packet.error = code;

    CAN.beginPacket(CAN_ID::ERR);
    CAN.write((uint8_t*)&packet, sizeof(packet));
    CAN.endPacket();
}

void sendAck()
{
    CAN.beginPacket(CAN_ID::ACK, 1, true);
    CAN.endPacket();
}

void sendMeasurement(Srf02& sensor)
{
    SENSOR_MEASUREMENT_M packet;

    packet.sensorId = sensor.sensorId();
    packet.unit = sensor.unit();
    Srf02::Status ret_ = sensor.oneShot(packet.range);

    if(ret_ != Srf02::Status::ok)
    {
        sendError(ret_);
    }
    else 
    {
        CAN.beginPacket(CAN_ID::SENSOR_MEASUREMENT);
        CAN.write((uint8_t*)&packet, sizeof(packet));
        CAN.endPacket();
    }
}

template<typename T>
void readMessage(T& packet)
{
    uint8_t buffer[sizeof(T)] = { 0 };
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
template void readMessage<SENSOR_MEASUREMENT_M>(SENSOR_MEASUREMENT_M&);
template void readMessage<ERR_M>(ERR_M&);
template void readMessage<LIST_SENSORS_RESPONSE_M>(LIST_SENSORS_RESPONSE_M&);
template void readMessage<STATUS_RESPONSE_M>(STATUS_RESPONSE_M&);
template void readMessage<STATUS_M>(STATUS_M&);