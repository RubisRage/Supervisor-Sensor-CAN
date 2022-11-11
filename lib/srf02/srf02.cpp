#include <srf02.hpp>
#include <Wire.h>
#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

bool Srf02::initialized = false;
SAMDTimer Srf02::timer_(SELECTED_TIMER);
SAMD_ISR_Timer Srf02::ISR_timer_;

Srf02::Srf02(uint8_t address) : address_(address >> 1),
                                delay_ms_(MINIMUM_DELAY_MS),
                                unit_(Srf02::Unit::cm),
                                last_measurement_ms_(millis()),
                                callback_(nullptr),
                                onPeriod_(false)
{}

Srf02::~Srf02() {}

Srf02::Status Srf02::begin() 
{
    if(!Srf02::initialized) 
    {
        Wire.begin();

        bool ret_ = Srf02::timer_.attachInterruptInterval_MS(
            HW_TIMER_INTERVAL_MS, 
            Srf02::TimerHandler
        );

        if (!ret_) 
            return Srf02::Status::timer_error;
        
        Srf02::initialized = true;

        return Srf02::Status::ok;
    }

    return Srf02::Status::already_initialized;
}

void Srf02::setUnit() 
{
    Wire.beginTransmission(address_);
    Wire.write(Srf02::Register::command_register);
    Wire.write(unit_); 
    Wire.endTransmission();
}

Srf02::Status Srf02::readRange(uint16_t &range)
{
    if (sinceLastMeasurement() < delay_ms_)
        return Srf02::Status::delay_not_acomplished;

    setUnit();

    uint8_t range_high = read_register(Srf02::Register::range_high_byte);
    uint8_t range_low  = read_register(Srf02::Register::range_low_byte);

    range = range_high << 8 | range_low;

    last_measurement_ms_ = millis();

    return Srf02::Status::ok;
}

void Srf02::callbackDispatcher(void* arg)
{
    Srf02* sensor = (Srf02*) arg;
    
    uint16_t range;
    sensor->readRange(range);

    sensor->callback_(range);
}

Srf02::Status Srf02::off()
{
    if(onPeriod_)
    {
        Srf02::ISR_timer_.disable(timerId_);
        onPeriod_ = false;
    }
}

Srf02::Status Srf02::oneShot(uint16_t& range)
{
    off();
    readRange(range);
}

Srf02::Status Srf02::onPeriod(
    uint16_t period_ms, 
    Srf02::callback_t callback
)
{
    if(callback == nullptr && callback_ == nullptr)
        return Srf02::Status::no_callback;

    if(callback != nullptr) 
        callback_ = callback;

    period_ms_ = period_ms;

    timerId_ = Srf02::ISR_timer_.setInterval(1000UL, Srf02::callbackDispatcher, (void*)this);

    onPeriod_ = true;

    return Srf02::Status::ok;
}

Srf02::Status Srf02::delay(uint16_t delay_ms)
{
    if (delay_ms < Srf02::MINIMUM_DELAY_MS)
        return Srf02::Status::delay_too_small;

    delay_ms_ = delay_ms;

    return Srf02::Status::ok;
}

uint8_t Srf02::read_register(Srf02::Register srf02_register) 
{
    Wire.beginTransmission(address_);
    Wire.write(srf02_register);
    Wire.endTransmission();
    
    unsigned long startTime_ms = millis();
    unsigned long timeout_ms = 500;
    
    Wire.requestFrom(address_, 1);

    while(!Wire.available()) { 
        /* Wait for Srf02 to respond */ 

        if((millis() - startTime_ms) > timeout_ms)
            return -1;
    }

    return Wire.read();
}


void Srf02::TimerHandler() 
{
    Srf02::ISR_timer_.run();
}