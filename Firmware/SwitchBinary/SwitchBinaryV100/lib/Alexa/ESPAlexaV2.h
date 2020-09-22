/*******************************************************************************
*File Name: ESPAlexa.h
*
* Version: 1.0
*
* Description:
* In this header file for Alexa functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#ifndef ESPAlexa_h
#define ESPAlexa_h
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h> 
#include <ArduinoJson.h> 
#include <StreamString.h>

#define DEVICE_ON   "ON"
#define DEVICE_OFF   "OFF"

class ESPAlexa{
public:
    String sAlexaAPIKey;
    String sSBRelayId;
    String sSBRelayName;

    ESPAlexa(void);
    //void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
    void Start(void);
    void handleRequest(void);
    void Update_All_Key_And_ID(void);
    void Update_Specified_File(uint8_t ui8ID);
private:
    WebSocketsClient webSocket;
    WiFiClient client;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPAlexa MAlexa;
#endif
#endif
