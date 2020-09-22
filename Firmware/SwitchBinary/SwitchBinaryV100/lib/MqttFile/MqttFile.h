/*******************************************************************************
*File Name: MQTTFile.h
*
* Version: 1.0
*
* Description:
* In this header used for managing mqtt files in spiffs system
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#ifndef MQTTFILE_h
#define MQTTFILE_h
#include <Arduino.h>

class MqttFile{
public:
    MqttFile();
    
    String sMqttPubRelayTopic;
    String sMqttSubRelayTopic;

    String sMqttPubRTCTopic;
    String sMqttSubRTCTopic;

    String sMqttPubAlarmTopic;
    String sMqttSubAlarmTopic;

    String sMqttPubOTAlarmTopic;
    String sMqttSubOTAlarmTopic;

    String sMqttPubAlarm0Topic;
    String sMqttSubAlarm0Topic;
    String sMqttPubAlarm1Topic;
    String sMqttSubAlarm1Topic;
    String sMqttPubAlarm2Topic;
    String sMqttSubAlarm2Topic;
    String sMqttPubAlarm3Topic;
    String sMqttSubAlarm3Topic;
    String sMqttPubAlarm4Topic;
    String sMqttSubAlarm4Topic;
    String sMqttPubAlarm5Topic;
    String sMqttSubAlarm5Topic;
    String sMqttPubAlarm6Topic;
    String sMqttSubAlarm6Topic;
    String sMqttPubAlarm7Topic;
    String sMqttSubAlarm7Topic;
    String sMqttPubAlarm8Topic;
    String sMqttSubAlarm8Topic;
    String sMqttPubAlarm9Topic;
    String sMqttSubAlarm9Topic;
    String sMqttPubAlarm10Topic;
    String sMqttSubAlarm10Topic;
    String sMqttPubAlarm11Topic;
    String sMqttSubAlarm11Topic;
    String sMqttPubAlarm12Topic;
    String sMqttSubAlarm12Topic;
    String sMqttPubAlarm13Topic;
    String sMqttSubAlarm13Topic;
    String sMqttPubAlarm14Topic;
    String sMqttSubAlarm14Topic;
    String sMqttPubAlarm15Topic;
    String sMqttSubAlarm15Topic;

    String sMqttPubTCTopic;
    String sMqttSubTCTopic;

    String sMqttDeviceStatus;

    String sMqttServer;
    uint16_t ui16MqttPort;
    String sMqttUsername;
    String sMqttPassword;
    String sMqttClientId;

    uint8_t  ui8MqttQoS;
    uint16_t ui16MqttKeepalive;

    void Read_Mqtt_Parameter(uint8_t ui8ID);
    void Read_All_Mqtt_Parameters(void);
    String Get_Value_Of_Mqtt_File(uint8_t ui8Index);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern MqttFile MqFile;
#endif
#endif