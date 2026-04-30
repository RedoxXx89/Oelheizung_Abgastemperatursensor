#include "exhaustTemperatureSensor.h"

EXHAUST_SENSOR_CONTROL::EXHAUST_SENSOR_CONTROL(uint8_t csPin)
    : _sensor(csPin),
      _temperature(-1.0f),
      _lastReadMs(0)
{
}

void EXHAUST_SENSOR_CONTROL::begin()
{
    _sensor.begin(MAX31865_3WIRE); // change to MAX31865_2WIRE or MAX31865_4WIRE if needed
}

void EXHAUST_SENSOR_CONTROL::execute()
{
    unsigned long now = millis();
    if (now - _lastReadMs >= EXHAUST_SENSOR_READ_INTERVAL_MS)
    {
        _lastReadMs = now;

        uint8_t fault = _sensor.readFault();
        if (fault != 0u)
        {
            _sensor.clearFault();
            _temperature = -1.0f;
        }
        else
        {
            _temperature = _sensor.temperature(MAX31865_RNOMINAL, MAX31865_RREF);
        }
    }
}

float EXHAUST_SENSOR_CONTROL::getTemperature() const
{
    return _temperature;
}
