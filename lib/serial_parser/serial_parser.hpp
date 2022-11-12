#pragma once

#include <cstdlib>
#include <sys/types.h>
#include <cstdint>
#include <srf02_config.hpp>

class Parser 
{

public:

    // SerialParser()

    enum Command : int8_t
    {
        none = 0,
        help,
        list_sensors,
        get_status,
        set_delay,
        set_unit,
        set_operation_mode,
    };

    Command parseCommand(char* toParse);

    Command command();

    inline Srf02Config::OperationMode operationMode() { return operationMode_; }
    inline uint16_t period() { return period_ms_; }
    inline uint16_t delay() { return delay_ms_; }
    inline Srf02Config::Unit unit() { return unit_; }
    inline uint8_t sensorId() { return sensorId_; }

private:

    Command lastCommand_ = Command::none;
    Srf02Config::OperationMode operationMode_;
    uint16_t period_ms_;
    uint16_t delay_ms_;
    Srf02Config::Unit unit_;
    int8_t sensorId_;

    Command parseSensorCommand();
};