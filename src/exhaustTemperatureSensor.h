#pragma once

#include <Arduino.h>
#include <Adafruit_MAX31865.h>

// PT100 reference resistor and nominal resistance
#define MAX31865_RREF 430.0f
#define MAX31865_RNOMINAL 100.0f

// Interval between sensor readings in milliseconds
#define EXHAUST_SENSOR_READ_INTERVAL_MS 1000UL

class EXHAUST_SENSOR_CONTROL
{
public:
    // csPin: chip-select pin for SPI
    explicit EXHAUST_SENSOR_CONTROL(uint8_t csPin);

    // Call once in setup()
    void begin();

    // Call regularly in loop() — reads the sensor every EXHAUST_SENSOR_READ_INTERVAL_MS
    void execute();

    // Returns the last measured temperature in °C, or -1.0 on fault
    float getTemperature() const;

private:
    Adafruit_MAX31865 _sensor;
    float _temperature;
    unsigned long _lastReadMs;
};
