#include <srf02.hpp>
#include <Wire.h>

bool Srf02::i2c_initialized = false;

Srf02::Srf02(uint8_t address) : address_(address >> 1),
                                delay_ms_(MINIMUM_DELAY_MS),
                                unit_(Srf02::Unit::cm),
                                last_measurment_ms_(millis()),
                                callback_(nullptr)
{
    if(!Srf02::i2c_initialized) 
    {
        Wire.begin();
        i2c_initialized = true;
    }
}

Srf02::~Srf02() {}


void Srf02::setUnit() 
{
    Wire.beginTransmission(address_);
    Wire.write(enum2value<Register, uint8_t>(Srf02::Register::command_register));
    Wire.write(enum2value<Unit, uint8_t>(unit_)); 
    Wire.endTransmission();
}

Srf02::Status Srf02::readRange(int16_t &range)
{
    if (sinceLastMeasurment() < delay_ms_)
        return Srf02::Status::delay_not_acomplished;

    setUnit();

    uint8_t range_high = read_register(Srf02::Register::range_high_byte);
    uint8_t range_low  = read_register(Srf02::Register::range_low_byte);

    range = range_high << 8 | range_low;

    last_measurment_ms_ = millis();

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
    Wire.write(enum2value<Srf02::Register, uint8_t>(srf02_register));
    Wire.endTransmission();
    
    // TODO: Add timeout to prevent infinite wait time 
    Wire.requestFrom(address_, 1);
    while(!Wire.available()) { /* Wait for Srf02 to respond */ }
    return Wire.read();
}

template<typename enum_type, typename cast_type>
cast_type Srf02::enum2value(enum_type enum_instance) {
    return static_cast<typename std::underlying_type<enum_type>::type>(enum_instance);
}