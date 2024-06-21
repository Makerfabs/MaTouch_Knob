#ifndef __WIFIASYNCWEBSERVER_H__
#define __WIFIASYNCWEBSERVER_H__

#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ESPmDNS.h>
#include "ArduinoJson.h"
#include <Preferences.h>
#include <Update.h>
#include "lvgl.h"
#include "ui.h"
#define FILESYSTEM FFat
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM_IF_FAILED true
#define Serial Serial

#if FILESYSTEM == FFat
#include <FFat.h>
#endif
#if FILESYSTEM == SPIFFS
#include <SPIFFS.h>
#endif

#define MAX_STA_COUNT 20   //0.5*20 = 10s
extern AsyncWebServer *server;  // initialise webserver
void wifi_server_begin(void *parameter);
void ffat_init();
#endif