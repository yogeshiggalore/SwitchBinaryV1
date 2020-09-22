/*******************************************************************************
*File Name: ESPAlexa.cpp
*
* Version: 1.0
*
* Description:
* In this source code for alexa usage
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/

#include <ESPAlexaV2.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h> 
#include <StreamString.h>
#include <ESPUtils.h>
#include <FS.h>
#include <ESPFile.h>
#include <Application.h>

#define HEARTBEAT_INTERVAL 300000

uint8_t ui8IsConnected=0;
uint64_t heartbeatTimestamp = 0;

ESPAlexa::ESPAlexa(void):webSocket(),client(){

}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){
    switch(type) {
        case WStype_DISCONNECTED:
            ui8IsConnected = false;
            EApp.ui8AlexaStatus = 0;
            EUtils.println("[WSc] Webservice disconnected from sinric.com!\n");
            break;
        case WStype_CONNECTED:
            ui8IsConnected = true;
            EApp.ui8AlexaStatus = 1;
            EUtils.println("[WSc] Service connected to sinric.com at url: " );
            //EUtils.println(String(payload).);
            EUtils.println("Waiting for commands from sinric.com ...\n");
            break;
        case WStype_ERROR:
            EUtils.println("WStype_ERROR");
            break;
        case WStype_FRAGMENT_TEXT_START:
            EUtils.println("WStype_FRAGMENT_TEXT_START");
            break;
        case WStype_FRAGMENT_BIN_START:
            EUtils.println("WStype_FRAGMENT_BIN_START");
            break;
        case WStype_FRAGMENT:
            EUtils.println("WStype_FRAGMENT");
            break;
        case WStype_FRAGMENT_FIN:
            EUtils.println("WStype_FRAGMENT_FIN");
            break;
        case WStype_TEXT:
            //Action_Selector(payload);
            EUtils.println("Sinric Data received...");
            EApp.HandleAlexa(payload);
            break;
        case WStype_BIN:
            EUtils.print("[WSc] get binary length: ");
            EUtils.println(length);
            break;
    }
}

void ESPAlexa::Start(void){

    Update_All_Key_And_ID();

    // server address, port and URL
    webSocket.begin("iot.sinric.com", 80, "/");
    // event handler
    webSocket.onEvent(webSocketEvent);
    //webSocket.setAuthorization("apikey", MyApiKey);
    
    webSocket.setAuthorization("apikey", sAlexaAPIKey.c_str());
    
    // try again every 5000ms if connection has failed
    webSocket.setReconnectInterval(5000);
}

void ESPAlexa::handleRequest(void){
    webSocket.loop();
    if(ui8IsConnected) {
        uint64_t now = millis();
        // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
        if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
            heartbeatTimestamp = now;
            webSocket.sendTXT("H");
            EUtils.println("sinric heartbeat");
        }
    }
}

void ESPAlexa::Update_Specified_File(uint8_t ui8ID){
    String sString;

    sString = EFile.Read_Data_From_File(sESPFiles[ui8ID]);
    EUtils.println(sString);

    if(ui8ID == ALEXA_TOKEN_INDEX){
        sAlexaAPIKey = sString;
    }else if(ui8ID == ALEXA_SWBIN_ID_INDEX){
        sSBRelayId = sString;
    }else if(ui8ID == ALEXA_SWBIN_NAME_INDEX){
        sSBRelayName = sString;
    }else{
        EUtils.println("file error");
    }
}

void ESPAlexa::Update_All_Key_And_ID(void){
    
    sAlexaAPIKey = EFile.Read_Data_From_File(sESPFiles[ALEXA_TOKEN_INDEX]);
    EUtils.print("API Key:");
    EUtils.println(sAlexaAPIKey);
    
    sSBRelayId = EFile.Read_Data_From_File(sESPFiles[ALEXA_SWBIN_ID_INDEX]);
    EUtils.print("SwitchBinaryRelayId:");    
    EUtils.println(sSBRelayId);


    sSBRelayName = EFile.Read_Data_From_File(sESPFiles[ALEXA_SWBIN_NAME_INDEX]);
    EUtils.print("SwitchBinaryRelayName:"); 
    EUtils.println(sSBRelayName);

}
// Preinstantiate Objects //////////////////////////////////////////////////////

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPAlexa MAlexa;
#endif
