

#pragma once

#include <Arduino.h>
#include "timerControl.h"

namespace HANDLER
{
    const uint8_t PAR_PIN_CHIP_SELECT_EXHAUST_SENSOR = D2;
    const float PAR_TEMP_DELTA_THRESHOLD_DEG_C = 5.0f;
    // const uint32_t PAR_POST_INTERVAL_MS = 5UL * 60UL * 1000UL;
    const uint32_t PAR_POST_INTERVAL_MS = 30000; // for testing only, set to 30 seconds, comment out when using real sensor data
}

class HANDLER_CONTROL
{
public:
    HANDLER_CONTROL();

    void execute(void);

private:
    void postData(void);
    bool temperatureChangedSignificantly(float currentTemperature) const;

    float _lastPostedTemperature;
    float currentTemperature;
    TIMER_CONTROL _postTimer;
};
