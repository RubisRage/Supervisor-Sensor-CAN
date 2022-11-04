#pragma once /* SRF02_H */

#include <cstdint>
#include <Arduino.h>
#include "SAMDTimerInterrupt.hpp"
#include "SAMD_ISR_Timer.hpp"

#define SELECTED_TIMER TIMER_TC5
#define HW_TIMER_INTERVAL_MS 50L


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
     * Definition of success or error return values.
    */
    enum Status : uint8_t
    {
        ok,                         /* Succesfull */
        delay_not_acomplished,      /* Tried taking measurement before specified delay time */
        delay_too_small,            /* Tried setting delay time to a value lower than MINIMUM_DELAY_MS */
        no_callback,                /* Tried setting period without specifying a callback */
        timer_error,                /* Failed to initialize hardware timer for periodic measurments */
        already_initialized         /* Common dependencies already initialized */
    };

    /**
     * Initializes Wire object, necessary to access the I2C bus. This function must be called
     * before any Srf02 instance is ready to be used.
    */
    static Status begin(void);


    /**
     * Returns the I2C address of the sensor.
     * 
     * @return Sensor's I2C address.
    */
    inline int8_t address(void) { return address_; };


    /**
     * Definition of the Srf02 ultrasonic sensor unit selection codes.
    */
    enum Unit : uint8_t 
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
    Status readRange(uint16_t& range);


    /**
     * On period callback type 
    */
    typedef void (*callback_t) (uint16_t);

    /**
     *
     * 
     * @return Success status, either of:
     *         - ok
     *         - no_callback
    */
    Status onPeriod(uint16_t period_ms, callback_t callback = nullptr);


    /**
     * Minimum value for this sensor's delay time.
    */
    static constexpr uint16_t MINIMUM_DELAY_MS = 65;


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
    uint8_t address_;                           /* Sensor's I2C address              */
    uint16_t delay_ms_;                         /* Sensor's current delay time       */
    Unit unit_;                                 /* Sensor's current measurement unit */
    unsigned long last_measurement_ms_;         /* Time of last measurement          */

    uint16_t period_ms_;                        /* Time period between periodic measurement    */
    callback_t callback_;                       /* Handler function for periodic measurements  */
    static SAMDTimer timer_;                    /* Timer for monitoring periodic measurements  */
    static SAMD_ISR_Timer ISR_timer_;           /* ISR_Timer for serving each periodic handler */
    
    /* Whether common dependencies have been initialized (I2C, SAMDTimer)*/
    static bool initialized;                

private:

    /**
     * Computes time passed since last measurement.
     * 
     * @return Time passed since last measurement.
    */
    inline unsigned long sinceLastMeasurement() { return millis() - last_measurement_ms_; }


    /**
     * Definition of Srf02 register's and their access codes.
    */
    enum Register : uint8_t 
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

    static void callbackDispatcher(void*);

    /**
     * Run ISR Timer on every HW timer interrupt 
    */
    static void TimerHandler();

};