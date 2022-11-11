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
        none = -1,
        help,
        list_sensors,
        get_status,
        set_delay,
        set_unit,
        set_operation_mode,
    };

    enum class Error : int8_t
    {
        number_expected,
        unknown_unit,
        unknown_command
    };

    Command parseCommand(char* toParse);

    Command command();

    inline Srf02::OperationMode operationMode() { return operationMode_; }
    inline uint16_t period() { return period_ms_; }
    inline uint16_t delay() { return delay_ms_; }
    inline Srf02::Unit unit() { return unit_; }
    inline uint8_t sensorId() { return sensorId_; }

private:

    Command lastCommand_ = Command::none;
    Srf02::OperationMode operationMode_;
    uint16_t period_ms_;
    uint16_t delay_ms_;
    Srf02::Unit unit_;
    int8_t sensorId_;

    Command parseSensorCommand();
};