#pragma once

namespace Srf02Config
{
    enum OperationMode
    {
        one_shot = 0,
        on_period,
        off
    };

    /**
     * Definition of the Srf02 ultrasonic sensor unit selection codes.
    */
    enum Unit : uint8_t 
    {
        inc = 80,
        cm  = 81,
        ms  = 82
    };

};
