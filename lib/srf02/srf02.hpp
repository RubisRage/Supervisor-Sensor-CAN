#pragma once /* Guard header file */

#include <cstdint>
#include <functional>
#include <Arduino.h>

/**
 * This class represents an intance of a Srf02 ultrasonic sensor connected via I2C.
 * 
 * @note This class takes control of the Wire object, it should not be used outside 
 *       this class. 
*/
class Srf02
{
public:
    Srf02(uint8_t address);
    ~Srf02();

    /**
     * Returns the I2C address of the sensor.
     * 
     * @return Sensor's I2C address.
    */
    inline int8_t address(void) { return address_; };

    /**
     * Definition of the Srf02 ultrasonic sensor unit selection codes.
    */
    enum class Unit : uint8_t 
    {
        inc = 80,
        cm  = 81,
        ms  = 82
    };

    /**
     * Sets the unit in which measurements should be taken.
     * 
     * @param[in] unit used to take measurments from calling moment onwards.
    */
    inline void unit(Unit unit) { unit_ = unit; }

    /**
     * Gets the unit in which measurements are being taken.
     * 
     * @return Unit in which measurements are being taken. 
    */
    inline Unit unit(void) { return unit_; }

    // enum class OperationMode : uint8_t
    // {
    //     one_shot = 1,
    //     on_period,
    //     off
    // };

    /**
     * Definition of success or error return values.
    */
    enum class Status : uint8_t
    {
        ok,                         /* Succesfull */
        delay_not_acomplished,      /* Tried taking measurement before specified delay time */
        delay_too_small             /* Tried setting delay time to a value lower than MINIMUM_DELAY_MS */
    };

    /**
     * Take a distance measurement from the sensor. The returned value's unit depends on the previously set 
     * unit, defaults to cm.
     * 
     * @param[out] range Variable where to store the taken measurement in case of success.
     * 
     * @return Success status, either of:
     *         - ok
     *         - delay_not_acomplished
    */
    Status readRange(int16_t& range);

    /**
     * Minimum value for this sensor's delay time.
    */
    static constexpr int16_t MINIMUM_DELAY_MS = 65;

    /**
     * Sets the delay for the sensor. This delay specifies the minimum time required to take between
     * two consecutive measurements.
     * 
     * @param[in] delay to be set. Must be greater than MINIMU_DELAY_MS.
     * 
     * @return Sucess status, either of:
     *          - ok
     *          - delay_too_small
    */
    Status delay(uint16_t delay);

    /**
     * Sets the delay for the sensor. This delay specifies the minimum time required to take between
     * two consecutive measurements.
     * 
     * @param[in] delay to be set. Must be greater than MINIMU_DELAY_MS.
     * 
     * @return Sucess status, either of:
     *          - ok
     *          - delay_too_small
    */
    inline uint16_t delay(void) { return delay_ms_; }

private:
    uint8_t address_;                           /* Sensor's I2C address */
    uint16_t delay_ms_;                         /* Sensor's current delay time */
    Unit unit_;                                 /* Sensor's current measurement unit */
    unsigned long last_measurment_ms_;          /* Time of last measurement */

    // OperationMode op_mode_;
    std::function<void(int16_t)> callback_;     /* TODO: Study whether to implement operationModes in this class */

    static bool i2c_initialized;                /* Whether the Wire object has been initialized */

private:

    /**
     * Computes time passed since last measurment.
     * 
     * @return Time passed since last measurment.
    */
    inline unsigned long sinceLastMeasurment() { return millis() - last_measurment_ms_; }


    /**
     * Definition of Srf02 register's and their access codes.
    */
    enum class Register : uint8_t 
    {
        command_register            =    0,
        range_high_byte             =    2,
        range_low_byte              =    3,
        autotune_minimum_high_byte  =    4,
        autotune_minimum_low_byte   =    5
    };

    /**
     * Notifies the Srf02 sensor to use the currently set unit to make the next measurment.
     * 
     * @note Should be called each time before reading the high and low bytes of the range register.
    */
    void setUnit(void);

    /**
     * Reads one of the Srf02 sensor's registers.
     * TODO: Add reading timeout to prevent blocking on unexpected cases
     * 
     * @param[in] srf02_register specifying which register to read.
     *
     * @return The read value.
    */
    uint8_t read_register(Register srf02_register);

    /**
     * Helper function to cast an enum class to it's underlying value.
    */
    template<typename enum_type, typename cast_type>
    static cast_type enum2value(enum_type enum_instance);
};