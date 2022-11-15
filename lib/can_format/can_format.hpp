#pragma once

#include <cstdint>
#include <srf02_config.hpp>
#include <srf02.hpp>

#define CAN_ID_OFFSET 10

extern const char* const errorMsg[]; 

enum CAN_ERR : uint8_t 
{
    delay_not_acomplished = Srf02::Status::delay_not_acomplished,       /* Tried taking measurement before specified delay time */
    delay_too_small,                                                    /* Tried setting delay time to a value lower than MINIMUM_DELAY_MS */
    period_too_small,                                                   /* Tried setting period time to a value lower than the set delay */
    timer_error,                                                        /* Failed to initialize hardware timer for periodic measurments */
    already_initialized,                                                /* Common dependencies already initialized */
    timeout,
    unknown_sensor                                                             /* Connection timed out when connecting to the srf02 sensor */
};

enum CAN_ID : uint8_t
{
    ACK = CAN_ID_OFFSET,
    ERR,
    OPMODE,
    UNIT,
    DELAY,
    STATUS,
    STATUS_RESPONSE,
    LIST,
    LIST_RESPONSE,
    SENSOR_MEASUREMENT
};

struct ERR_M 
{
    uint8_t error;
};

struct __attribute__((packed)) OPMODE_M 
{
    uint8_t sensorId;
    Srf02Config::OperationMode opMode;
    uint16_t period_ms;
};

struct UNIT_M 
{
    uint8_t sensorId;
    Srf02Config::Unit unit;
};

struct DELAY_M 
{
    uint8_t sensorId;
    uint16_t delay_ms;
};

struct STATUS_M
{
    uint8_t sensorId;
};

struct __attribute__((packed)) STATUS_RESPONSE_M
{
    uint8_t sensorId;
    uint16_t i2cAddr;
    uint16_t delay;
    Srf02Config::Unit unit;
    uint16_t period_ms;
};

struct LIST_SENSORS_RESPONSE_M 
{
    uint8_t sensorId1;
    uint8_t sensorId2;
};

struct SENSOR_MEASUREMENT_M
{
    uint8_t sensorId;
    Srf02Config::Unit unit;
    uint16_t range;
};


template<typename T>
void readMessage(T& packet);
void sendAck();
void sendError(uint8_t code);
void sendMeasurement(Srf02& sensor);