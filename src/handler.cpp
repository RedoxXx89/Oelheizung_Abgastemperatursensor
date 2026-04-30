#include <Arduino.h>
#include "handler.h"
#include "wireless.h"
#include "exhaustTemperatureSensor.h"

#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

using namespace HANDLER;

static EXHAUST_SENSOR_CONTROL exhaustSensor(PAR_PIN_CHIP_SELECT_EXHAUST_SENSOR);
WIRELESS_CONTROL *wirelessControl;

const char *api_serverName = "http://marcelhausraspberripi/post_oilheater_data.php";
const char *apiKey = "api_key";
const char *apiKeyValue = "tPmAT5Ab3j7F9";

const char *apiKey_exhaustTemperature = "abgastemperatur";

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

HANDLER_CONTROL::HANDLER_CONTROL()
    : _lastPostedTemperature(-1.0f),
      _postTimer(PAR_POST_INTERVAL_MS)
{
    Serial.begin(115200);
    exhaustSensor.begin();
    wirelessControl = new WIRELESS_CONTROL();
}

void HANDLER_CONTROL::execute()
{
    exhaustSensor.execute();
    wirelessControl->execute();

    // this->currentTemperature = exhaustSensor.getTemperature();
    this->currentTemperature = 20; // for testing only, comment out when using real sensor data
    const bool sensorFault = (currentTemperature < 0.0f);

    if (!sensorFault)
    {
        Serial.print("Current temperature: ");
        Serial.print(currentTemperature);
        Serial.println(" °C");

        const bool postDueToTimerElapsed = _postTimer.isDone();
        const bool postDueToTempChange = temperatureChangedSignificantly(currentTemperature);

        if (postDueToTimerElapsed || postDueToTempChange)
        {
            postData();
            _lastPostedTemperature = currentTemperature;
            _postTimer.reset();
        }
    }
    else
    {
        Serial.println("Sensor fault detected!");
    }
    delay(500);
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

bool HANDLER_CONTROL::temperatureChangedSignificantly(float currentTemperature) const
{
    const float delta = fabsf(currentTemperature - _lastPostedTemperature);
    return delta >= PAR_TEMP_DELTA_THRESHOLD_DEG_C;
}

void HANDLER_CONTROL::postData()
{
    String mySqlPostString = String(apiKey) + "=" + String(apiKeyValue);

    mySqlPostString += "&" + String(apiKey_exhaustTemperature) + "=" + String(this->currentTemperature);

    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, api_serverName);

        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Prepare your HTTP POST request data
        String httpRequestData = mySqlPostString;
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);

        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();

        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
    }
}
