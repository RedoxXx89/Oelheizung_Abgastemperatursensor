/*
oelheizung fuellstandssensor wifi handler
marcel riebel 07.02.2025
*/

#pragma once

#define PAR_WIFI_ENABLE true

#define PAR_WIFI_ENABLE_CAPTIVE_PORTAL true // enable captive portal
#define PAR_WIFI_SET_DARK_MODE_ON true      // enable dark mode
// #define PAR_WIFI_CONFIG_PORTAL_TIMEOUT 15         // seconds till captive portal closes again
#define PAR_WIFI_CONFIG_PORTAL_TIMEOUT 15         // seconds till captive portal closes again
#define PAR_WIFI_SET_CONFIG_PORTAL_BLOCKING false // set if the config portal shall be blocking
#define PAR_WIFI_CONNECTION_TIMEOUT 5             // seconds till connection timeout detected
#define PAR_WIFI_CONNECTION_RETRIES 3             // retries before captive portal opens

#define PAR_WIFI_CONFIG_NTP_UPDATE_INTERVAL 3600000 // ms every hour update
#define PAR_WIFI_CONFIG_NTP_TIME_OFFSET 3600        // sek ->gmt+1h germany wintertime / normaltime

typedef enum
{
    SELECT_WIFI_STATE_CONNECTED,
    SELECT_WIFI_STATE_ACCESS_POINT,
    SELECT_WIFI_STATE_NOT_CONNECTED,
} SELECT_WIFI_STATE;

class WIRELESS_CONTROL
{
public:
    WIRELESS_CONTROL();

    void execute(void);

    bool isConnectedToWifi(void);

private:
    void update(void);
    void updateWifiStatus(void);
    SELECT_WIFI_STATE currentWifiStatus;
    SELECT_WIFI_STATE lastWifiStatus;

    void configureWifi(void);
    void configureCaptivePortal(void);

    void initializeOTA(void);
};