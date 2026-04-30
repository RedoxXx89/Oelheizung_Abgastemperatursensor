/*
oelheizung fuellstandssensor wifi handler
marcel riebel 07.02.2025
*/

#include "Arduino.h"
#include "wireless.h"
#include "wireless_secrets.h"

#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char *wifi_device_name = "Riebel_Oeltank_Datensammler";
const char *wifi_device_OTA_name = "Riebel_Oeltank_Datensammler";

WiFiManager wm;

// class wireless control
WIRELESS_CONTROL::WIRELESS_CONTROL()
{
    this->configureWifi();
    this->configureCaptivePortal();

    if (PAR_WIFI_ENABLE)
    {
        wm.autoConnect(wifi_device_name);
    }

    currentWifiStatus = SELECT_WIFI_STATE_NOT_CONNECTED;
    lastWifiStatus = SELECT_WIFI_STATE_ACCESS_POINT;
}

void WIRELESS_CONTROL::execute()
{
    this->update();

    wm.process();
}

bool WIRELESS_CONTROL::isConnectedToWifi()
{
    bool targetStatus = false;

    if (currentWifiStatus == SELECT_WIFI_STATE_CONNECTED)
    {
        // connected to wifi
        targetStatus = true;
    } // no else needed

    return targetStatus;
}

// private
void WIRELESS_CONTROL::update()
{
    this->updateWifiStatus();

    bool isConnected = (currentWifiStatus == SELECT_WIFI_STATE_CONNECTED);
    bool wasConnected = (lastWifiStatus == SELECT_WIFI_STATE_CONNECTED);
    bool isApMode = (currentWifiStatus == SELECT_WIFI_STATE_ACCESS_POINT);
    bool isApClosed = (lastWifiStatus == SELECT_WIFI_STATE_ACCESS_POINT) && (currentWifiStatus == SELECT_WIFI_STATE_NOT_CONNECTED);

    // is access point
    if (isApMode)
    {
        // ota is NOT allowed to be operated in AP mode
        lastWifiStatus = currentWifiStatus;
    }
    else if (isApClosed)
    {
        // ap was open now try the stored network again
        if (PAR_WIFI_ENABLE)
        {
            wm.autoConnect(wifi_device_name);
        }
    }

    // is connected to wifi
    else if (isConnected)
    {
        if (!wasConnected)
        {
            // new connection
            this->initializeOTA();
        }
        else
        {
            // already connected
            ArduinoOTA.handle();
        }
        lastWifiStatus = SELECT_WIFI_STATE_CONNECTED;
    }
    // not connected to wifi
    else
    {
        lastWifiStatus = SELECT_WIFI_STATE_NOT_CONNECTED;
    }
}

void WIRELESS_CONTROL::updateWifiStatus()
{
    if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA)
    {
        currentWifiStatus = SELECT_WIFI_STATE_ACCESS_POINT;
    }
    else if ((WiFi.status() == WL_CONNECTED))
    {
        currentWifiStatus = SELECT_WIFI_STATE_CONNECTED;
    }
    else
    {
        currentWifiStatus = SELECT_WIFI_STATE_NOT_CONNECTED;
    }
}

void WIRELESS_CONTROL::configureWifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.hostname(wifi_device_name);

    wm.preloadWiFi(String(wirelessSecrets::ssid), String(wirelessSecrets::pass));
    wm.setConnectTimeout(PAR_WIFI_CONNECTION_TIMEOUT);
    wm.setConnectRetries(PAR_WIFI_CONNECTION_RETRIES);
}

void WIRELESS_CONTROL::configureCaptivePortal()
{
    wm.setCaptivePortalEnable(PAR_WIFI_ENABLE_CAPTIVE_PORTAL);
    wm.setConfigPortalBlocking(PAR_WIFI_SET_CONFIG_PORTAL_BLOCKING);
    wm.setConfigPortalTimeout(PAR_WIFI_CONFIG_PORTAL_TIMEOUT);

    wm.setDarkMode(PAR_WIFI_SET_DARK_MODE_ON);

    // Set custom AP IP address
    IPAddress apIP(8, 8, 8, 8);
    IPAddress gw(10, 0, 1, 1);
    IPAddress sn(255, 255, 255, 0);
    wm.setAPStaticIPConfig(apIP, gw, sn);
}

void WIRELESS_CONTROL::initializeOTA()
{
    ArduinoOTA.setHostname(wifi_device_OTA_name);
    ArduinoOTA.setPassword(wirelessSecrets::otaPass);

    ArduinoOTA.onStart([]()
                       {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    Serial.println("Start updating " + type); });

    ArduinoOTA.onEnd([]()
                     { Serial.println("\nEnd"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress * 100) / total); });

    ArduinoOTA.onError([](ota_error_t error)
                       {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

    ArduinoOTA.begin();
}