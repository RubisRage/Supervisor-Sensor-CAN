#pragma once /* SRF02_H */

#include <cstdint>
#include <Arduino.h>
#include <srf02_config.hpp>
#include <functional>

#define SELECTED_TIMER TIMER_TC5
#define HW_TIMER_INTERVAL_MS 50L
#define USING_TIMER_TC5 true

#ifdef SENSOR

#include "SAMDTimerInterrupt.hpp"
#include "SAMD_ISR_Timer.hpp"

#endif

/**
 * This class represents an intance of a Srf02 ultrasonic sensor connected via I2C.
 * 
 * @note This class takes control of the Wire object, it should not be used outside 
 *       this class. 
*/
class Srf02
{
public:
    Srf02(uint8_t address, uint8_t sensorId);
    ~Srf02();

    /**
     * Definition of success or error return values.
    */
    enum Status : uint8_t
    {
        ok,                         /* Succesfull */
        delay_not_acomplished,      /* Tried taking measurement before specified delay time */
        delay_too_small,            /* Tried setting delay time to a value lower than MINIMUM_DELAY_MS */
        period_too_small,           /* Tried setting period time to a value lower than the set delay */
        no_callback,                /* Tried setting period without specifying a callback */
        timer_error,                /* Failed to initialize hardware timer for periodic measurments */
        already_initialized,        /* Common dependencies already initialized */
        timeout                     /* Connection timed out when connecting to the srf02 sensor */
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
     * Sets the unit in which measurements should be taken.
     * 
     * @param[in] unit used to take measurments from calling moment onwards.
    */
    inline void unit(Srf02Config::Unit unit) { unit_ = unit; }


    /**
     * Gets the unit in which measurements are being taken.
     * 
     * @return Unit in which measurements are being taken. 
    */
    inline Srf02Config::Unit unit(void) const { return unit_; }

    /**
     * 
    */
   inline uint8_t sensorId() const { return sensorId_; }


    // TODO: Comment this functions 
    Status off();
    Status oneShot(uint16_t& range);

    typedef void (*callback_t)(void*);

    /**
     *
     * 
     * @return Success status, either of:
     *         - ok
     *         - no_callback
    */
    Status onPeriod(uint16_t period_ms, callback_t callback);


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
     * TODO: Comment
    */
    inline uint16_t delay(void) { return delay_ms_; }


private:
    uint8_t address_;                           /* Sensor's I2C address              */
    uint16_t delay_ms_;                         /* Sensor's current delay time       */
    Srf02Config::Unit unit_;                    /* Sensor's current measurement unit */
    unsigned long last_measurement_ms_;         /* Time of last measurement          */
    bool onPeriod_;                             /* Flag specifying whether or not the onPeriod operation mode is on */

    int timerId_;                               /* SAMD_TimerInterrupt timer ID */
    uint16_t period_ms_;                        /* Time period between periodic measurement    */
    callback_t callback_;                       /* Handler function for periodic measurements  */
    uint8_t sensorId_;

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
     * Reads one of the Srf02 sensor's registers.
     * 
     * @param[in] srf02_register specifying which register to read.
     * @param[out] out variable in which the register is stored.
     *
     * @return 0 if successfull, 1 on timeout.
    */
    int read_register(Register srf02_register, uint8_t& out);
};