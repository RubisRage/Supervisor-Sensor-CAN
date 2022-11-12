#pragma once

#include <cstdint>
#include <srf02_config.hpp>

#define CAN_ID_OFFSET 10

enum CAN_ID : uint8_t
{
    ACK = 10,
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

struct OPMODE_M 
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