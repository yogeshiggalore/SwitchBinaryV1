/*******************************************************************************
*File Name: MqttFile.cpp
*
* Version: 1.0
*
* Description:
* In this source code for managing mqtt files
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/
#include <Arduino.h>
#include <FS.h>
#include <ESPUtils.h>
#include <ESPFile.h>
#include <MqttFile.h>
#include <ESPUtils.h>

// Constructors ////////////////////////////////////////////////////////////////
MqttFile::MqttFile(){}

void MqttFile::Read_Mqtt_Parameter(uint8_t ui8ID){
    String sString;

    sString = EFile.Read_Data_From_File(sESPFiles[ui8ID]);
    EUtils.println(sString);

    if(ui8ID == MQTT_PUB_RELAY_TOPIC_INDEX){
        sMqttPubRelayTopic = sString;
    }else if(ui8ID == MQTT_SUB_RELAY_TOPIC_INDEX){
        sMqttSubRelayTopic = sString;
    }else if(ui8ID == MQTT_PUB_RTC_TOPIC_INDEX){
        sMqttPubRTCTopic = sString;
    }else if(ui8ID == MQTT_SUB_RTC_TOPIC_INDEX){
        sMqttSubRTCTopic = sString;
    }else if(ui8ID == MQTT_PUB_ALARM_TOPIC_INDEX){
        sMqttPubAlarmTopic = sString;
    }else if(ui8ID == MQTT_SUB_ALARM_TOPIC_INDEX){
        sMqttSubAlarmTopic = sString;
    }else if(ui8ID == MQTT_PUB_TC_TOPIC_INDEX){
        sMqttPubTCTopic = sString;
    }else if(ui8ID == MQTT_SUB_TC_TOPIC_INDEX){
        sMqttSubTCTopic = sString;
    }else if(ui8ID == MQTT_SERVER_INDEX){
        sMqttServer = sString;
    }else if(ui8ID == MQTT_PORT_INDEX){
        ui16MqttPort = sString.toInt();
    }else if(ui8ID == MQTT_USERNAME_INDEX){
        sMqttUsername = sString;
    }else if(ui8ID == MQTT_PASSWORD_INDEX){
        sMqttPassword = sString;
    }else if(ui8ID == MQTT_CLIENTID_INDEX){
        sMqttClientId = sString;
    }else if(ui8ID == MQTT_QOS_INDEX){
        ui8MqttQoS = sString.toInt();
    }else if(ui8ID == MQTT_KEEPALIVE_INDEX){
        ui16MqttKeepalive = sString.toInt();
    }else{
        EUtils.println("file error");
    }
}

void MqttFile::Read_All_Mqtt_Parameters(void){
    String sDeviceName;

    sDeviceName = EUtils.Get_Device_Name() + "/";

    EUtils.println("Updating mqtt parameters");

    sMqttDeviceStatus = sDeviceName + "Status";

    
    sMqttPubRelayTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_PUB_RELAY_TOPIC_INDEX]);
    sMqttPubRelayTopic = sDeviceName + sMqttPubRelayTopic + "/Get";
    EUtils.print("sMqttPubRelayTopic:");    
    EUtils.println(sMqttPubRelayTopic);

    sMqttSubRelayTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_SUB_RELAY_TOPIC_INDEX]);
    sMqttSubRelayTopic = sDeviceName + sMqttSubRelayTopic + "/Set";
    EUtils.print("sMqttSubRelayTopic:");    
    EUtils.println(sMqttSubRelayTopic);

    sMqttPubRTCTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_PUB_RTC_TOPIC_INDEX]);
    sMqttPubRTCTopic = sDeviceName + sMqttPubRTCTopic + "/Get";
    EUtils.print("sMqttPubRTCTopic:");    
    EUtils.println(sMqttPubRTCTopic);

    sMqttSubRTCTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_SUB_RTC_TOPIC_INDEX]);
    sMqttSubRTCTopic = sDeviceName + sMqttSubRTCTopic + "/Set";
    EUtils.print("sMqttSubRTCTopic:");    
    EUtils.println(sMqttSubRTCTopic);

    sMqttPubAlarmTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_PUB_ALARM_TOPIC_INDEX]);
    sMqttPubAlarm0Topic = sDeviceName + sMqttPubAlarmTopic + "/0/Get";
    sMqttPubAlarm1Topic = sDeviceName + sMqttPubAlarmTopic + "/1/Get";
    sMqttPubAlarm2Topic = sDeviceName + sMqttPubAlarmTopic + "/2/Get";
    sMqttPubAlarm3Topic = sDeviceName + sMqttPubAlarmTopic + "/3/Get";
    sMqttPubAlarm4Topic = sDeviceName + sMqttPubAlarmTopic + "/4/Get";
    sMqttPubAlarm5Topic = sDeviceName + sMqttPubAlarmTopic + "/5/Get";
    sMqttPubAlarm6Topic = sDeviceName + sMqttPubAlarmTopic + "/6/Get";
    sMqttPubAlarm7Topic = sDeviceName + sMqttPubAlarmTopic + "/7/Get";
    sMqttPubAlarm8Topic = sDeviceName + sMqttPubAlarmTopic + "/8/Get";
    sMqttPubAlarm9Topic = sDeviceName + sMqttPubAlarmTopic + "/9/Get";
    sMqttPubAlarm10Topic = sDeviceName + sMqttPubAlarmTopic + "/10/Get";
    sMqttPubAlarm11Topic = sDeviceName + sMqttPubAlarmTopic + "/11/Get";
    sMqttPubAlarm12Topic = sDeviceName + sMqttPubAlarmTopic + "/12/Get";
    sMqttPubAlarm13Topic = sDeviceName + sMqttPubAlarmTopic + "/13/Get";
    sMqttPubAlarm14Topic = sDeviceName + sMqttPubAlarmTopic + "/14/Get";
    sMqttPubAlarm15Topic = sDeviceName + sMqttPubAlarmTopic + "/15/Get";

    sMqttPubOTAlarmTopic = sDeviceName + sMqttPubAlarmTopic + "/OT/Get";

    EUtils.print("sMqttPubAlarmTopic:");    
    EUtils.println(sMqttPubAlarm0Topic);
    EUtils.println(sMqttPubAlarm1Topic);
    EUtils.println(sMqttPubAlarm2Topic);
    EUtils.println(sMqttPubAlarm3Topic);
    EUtils.println(sMqttPubAlarm4Topic);
    EUtils.println(sMqttPubAlarm5Topic);
    EUtils.println(sMqttPubAlarm6Topic);
    EUtils.println(sMqttPubAlarm7Topic);
    EUtils.println(sMqttPubAlarm8Topic);
    EUtils.println(sMqttPubAlarm9Topic);
    EUtils.println(sMqttPubAlarm10Topic);
    EUtils.println(sMqttPubAlarm11Topic);
    EUtils.println(sMqttPubAlarm12Topic);
    EUtils.println(sMqttPubAlarm13Topic);
    EUtils.println(sMqttPubAlarm14Topic);
    EUtils.println(sMqttPubAlarm15Topic);
    EUtils.println(sMqttPubOTAlarmTopic);

    sMqttSubAlarmTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_SUB_ALARM_TOPIC_INDEX]);
    sMqttSubAlarm0Topic = sDeviceName + sMqttSubAlarmTopic + "/0/Set";
    sMqttSubAlarm1Topic = sDeviceName + sMqttSubAlarmTopic + "/1/Set";
    sMqttSubAlarm2Topic = sDeviceName + sMqttSubAlarmTopic + "/2/Set";
    sMqttSubAlarm3Topic = sDeviceName + sMqttSubAlarmTopic + "/3/Set";
    sMqttSubAlarm4Topic = sDeviceName + sMqttSubAlarmTopic + "/4/Set";
    sMqttSubAlarm5Topic = sDeviceName + sMqttSubAlarmTopic + "/5/Set";
    sMqttSubAlarm6Topic = sDeviceName + sMqttSubAlarmTopic + "/6/Set";
    sMqttSubAlarm7Topic = sDeviceName + sMqttSubAlarmTopic + "/7/Set";
    sMqttSubAlarm8Topic = sDeviceName + sMqttSubAlarmTopic + "/8/Set";
    sMqttSubAlarm9Topic = sDeviceName + sMqttSubAlarmTopic + "/9/Set";
    sMqttSubAlarm10Topic = sDeviceName + sMqttSubAlarmTopic + "/10/Set";
    sMqttSubAlarm11Topic = sDeviceName + sMqttSubAlarmTopic + "/11/Set";
    sMqttSubAlarm12Topic = sDeviceName + sMqttSubAlarmTopic + "/12/Set";
    sMqttSubAlarm13Topic = sDeviceName + sMqttSubAlarmTopic + "/13/Set";
    sMqttSubAlarm14Topic = sDeviceName + sMqttSubAlarmTopic + "/14/Set";
    sMqttSubAlarm15Topic = sDeviceName + sMqttSubAlarmTopic + "/15/Set";

    sMqttSubOTAlarmTopic = sDeviceName + sMqttSubAlarmTopic + "/OT/Set";

    EUtils.print("sMqttSubAlarmTopic:");    
    EUtils.println(sMqttSubAlarm0Topic);
    EUtils.println(sMqttSubAlarm1Topic);
    EUtils.println(sMqttSubAlarm2Topic);
    EUtils.println(sMqttSubAlarm3Topic);
    EUtils.println(sMqttSubAlarm4Topic);
    EUtils.println(sMqttSubAlarm5Topic);
    EUtils.println(sMqttSubAlarm6Topic);
    EUtils.println(sMqttSubAlarm7Topic);
    EUtils.println(sMqttSubAlarm8Topic);
    EUtils.println(sMqttSubAlarm9Topic);
    EUtils.println(sMqttSubAlarm10Topic);
    EUtils.println(sMqttSubAlarm11Topic);
    EUtils.println(sMqttSubAlarm12Topic);
    EUtils.println(sMqttSubAlarm13Topic);
    EUtils.println(sMqttSubAlarm14Topic);
    EUtils.println(sMqttSubAlarm15Topic);
    EUtils.println(sMqttSubOTAlarmTopic);

    sMqttPubTCTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_PUB_TC_TOPIC_INDEX]);
    sMqttPubTCTopic = sDeviceName + sMqttPubTCTopic + "/Get";
    EUtils.print("sMqttPubTCTopic:");    
    EUtils.println(sMqttPubTCTopic);

    sMqttSubTCTopic = EFile.Read_Data_From_File(sESPFiles[MQTT_SUB_TC_TOPIC_INDEX]);
    sMqttSubTCTopic = sDeviceName + sMqttSubTCTopic + "/Set";
    EUtils.print("sMqttSubTCTopic:");    
    EUtils.println(sMqttSubTCTopic);

    sMqttServer = EFile.Read_Data_From_File(sESPFiles[MQTT_SERVER_INDEX]);
    EUtils.print("MqttServer:"); 
    EUtils.println(sMqttServer);

    ui16MqttPort = EFile.Read_Data_From_File(sESPFiles[MQTT_PORT_INDEX]).toInt();
    EUtils.print("MqttPort:"); 
    EUtils.println(ui16MqttPort);

    sMqttUsername = EFile.Read_Data_From_File(sESPFiles[MQTT_USERNAME_INDEX]);
    EUtils.print("MqttUsername:"); 
    EUtils.println(sMqttUsername);

    sMqttPassword = EFile.Read_Data_From_File(sESPFiles[MQTT_PASSWORD_INDEX]);
    EUtils.print("MqttPassword:"); 
    EUtils.println(sMqttPassword);

    sMqttClientId = EFile.Read_Data_From_File(sESPFiles[MQTT_CLIENTID_INDEX]);
    EUtils.print("MqttClient:"); 
    if(strcmp("noclientid",sMqttClientId.c_str()) == 0){
        sMqttClientId = EUtils.Get_Device_Name();
    }
    EUtils.println(sMqttClientId);

    ui8MqttQoS = EFile.Read_Data_From_File(sESPFiles[MQTT_QOS_INDEX]).toInt();
    EUtils.print("MqttQoS:"); 
    EUtils.println(ui8MqttQoS);

    ui16MqttKeepalive = EFile.Read_Data_From_File(sESPFiles[MQTT_KEEPALIVE_INDEX]).toInt();
    EUtils.print("MqttKeepalive:"); 
    EUtils.println(ui16MqttKeepalive);

}

String MqttFile::Get_Value_Of_Mqtt_File(uint8_t ui8Index){
    String sString;

    if(ui8Index == MQTT_PUB_RELAY_TOPIC_INDEX){
        sString= sMqttPubRelayTopic;
    }else if(ui8Index == MQTT_SUB_RELAY_TOPIC_INDEX){
        sString= sMqttSubRelayTopic;
    }else if(ui8Index == MQTT_PUB_RTC_TOPIC_INDEX){
        sString= sMqttPubRTCTopic;
    }else if(ui8Index == MQTT_SUB_RTC_TOPIC_INDEX){
        sString= sMqttSubRTCTopic;
    }else if(ui8Index == MQTT_PUB_ALARM_TOPIC_INDEX){
        sString= sMqttPubAlarmTopic;
    }else if(ui8Index == MQTT_SUB_ALARM_TOPIC_INDEX){
        sString= sMqttSubAlarmTopic;
    }else if(ui8Index == MQTT_PUB_TC_TOPIC_INDEX){
        sString= sMqttPubTCTopic;
    }else if(ui8Index == MQTT_SUB_TC_TOPIC_INDEX){
        sString= sMqttSubTCTopic;
    }else if(ui8Index == MQTT_SERVER_INDEX){
        sString= sMqttServer;
    }else if(ui8Index == MQTT_PORT_INDEX){
        sString= String(ui16MqttPort);
    }else if(ui8Index == MQTT_USERNAME_INDEX){
        sString= sMqttUsername;
    }else if(ui8Index == MQTT_PASSWORD_INDEX){
        sString= sMqttPassword;
    }else if(ui8Index == MQTT_CLIENTID_INDEX){
        sString= sMqttClientId;
    }else if(ui8Index == MQTT_QOS_INDEX){
        sString= String(ui8MqttQoS);
    }else if(ui8Index == MQTT_KEEPALIVE_INDEX){
        sString= String(ui16MqttKeepalive);
    }else{
        sString= "  ";
    }

    return sString;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
MqttFile MqFile;
#endif