/*******************************************************************************
* Project Name: SwitchBinaryV100
*
* Version: 1.0
*
* Description:
* In this project ESP8266 controls relay
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , SwitchBinary
*******************************************************************************/

/* include headers */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESPFile.h>
#include <MqttFile.h>
#include <AsyncMqttClient.h>
#include <WifiControlV2.h>
#include <TaskScheduler.h>
#include <Application.h>
#include <ESPMemory.h>
#include <ESPAlexaV2.h>
#include <ESPUtils.h>
#include <ESPRTC.h>

uint8_t ui8MemoryLED1Value=0;

/* led web page index */
const char* htmlfile = "/index.html";

/* Static i config for server */
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,0);
IPAddress subnet(255,255,255,0);

/* webserver object on port 80 */
ESP8266WebServer ESPServer(80);

/* ESP Server functions*/
void ESPServer_HandleWebRequests(void);
void ESPServer_HandleRoot(void);
void ESPServer_SwicthBinaryUpdate(void);
void ESPServer_Handle_MqttParameter(void);
void ESPServer_Handle_WifiAdd(void);
void ESPServer_Handle_WifiHostname(void);
void ESPServer_Handle_Alexa(void);
void ESPServer_Handle_WifiParameterAdd(void);
void ESPServer_RelayUpdate(void);
void ESPServer_RTCUpdate(void);
void ESPServer_Handle_Timeconfig(void);
void ESPServer_Enable_Timeconfig(void);
bool ESPServer_LoadFromSpiffs(String path);
void ESPServer_Handle_DeviceReset(void);
void ESPServer_Mqtt_Values(void);
void ESPServer_Wifi_Values(void);
void ESPServer_Alexa_Values(void);
void ESPServer_Handle_OTTimeconfig(void);

/* timer and intruppt function for 1 milisecond */
void Timer_1ms_Start(void);
void ICACHE_RAM_ATTR timercallback1ms(void);

/* callbacks */
void TestCallback(void);
void WifiControlV1Callback(void);
void InternetCallback(void);
void MqttCallback(void);

/* Mqtt functions */
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);
void connectToMqtt(void);
void MQTT_Handle(void);
void Mqtt_Publish_All(void);

Task TestTask(1000, TASK_FOREVER, &TestCallback);
Task WifiControlTask(1000, TASK_FOREVER, &WifiControlV1Callback);
Task MqttControlTask(60000, TASK_FOREVER, &MqttCallback);
Task MqttReconnectTask(2000,TASK_ONCE,&connectToMqtt);


Scheduler TaskRunner;

/* timer flags */
uint8_t  ui81msFlag=0;
uint64_t ui64OnemsTimeCounter=0;
long lLastTime=0;
uint8_t ui8DeviceResetFlag=0;

// MQTT 
AsyncMqttClient mqttClient;
uint8_t ui8MqttSendFlag;
IPAddress IPAddr;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void setup() {
	
	delay(1000);

    Serial1.begin(921600);
    Serial1.println("SwitchBinaryV100");
    EUtils.Enable_Serial_Debug();
    
	EMemory.Init();
	EApp.Start();

    /* Initialize SPIFFS */
	if(!SPIFFS.begin()){
		Serial1.println("An Error has occurred while mounting SPIFFS");
		while(1);
	}

    /* start file system and read all files */
    EFile.Start();
	MqFile.Read_All_Mqtt_Parameters();

    Serial1.println("Connecting to wifi");

    EWifiControl.Start();
    IPAddr = WiFi.localIP();
    Serial1.println(IPAddr);

    EUtils.Start();

	/* Enable ESP Server function and start webserver */
    ESPServer.on("/",ESPServer_HandleRoot);
    ESPServer.on("/relayupdate",ESPServer_RelayUpdate);
	ESPServer.on("/switchbinaryupdate",ESPServer_SwicthBinaryUpdate);
    ESPServer.on("/addmqtt", ESPServer_Handle_MqttParameter);
    ESPServer.on("/addwifi", ESPServer_Handle_WifiAdd);
    ESPServer.on("/addwifihostname", ESPServer_Handle_WifiHostname);
    ESPServer.on("/addalexa",ESPServer_Handle_Alexa);
    ESPServer.on ("/addwifiparameter", ESPServer_Handle_WifiParameterAdd); 
	ESPServer.on ("/reset", ESPServer_Handle_DeviceReset);
    ESPServer.on ("/settime", ESPServer_RTCUpdate);
    ESPServer.on ("/timeconfig",ESPServer_Handle_Timeconfig);
    ESPServer.on ("/ottimeconfig",ESPServer_Handle_OTTimeconfig);
    ESPServer.on ("/enabletc",ESPServer_Enable_Timeconfig);
    ESPServer.on ("/getmqttvalues", ESPServer_Mqtt_Values);
    ESPServer.on ("/getwifivalues",ESPServer_Wifi_Values);
    ESPServer.on ("/getalexavalues",ESPServer_Alexa_Values);
    ESPServer.onNotFound(ESPServer_HandleWebRequests);
    ESPServer.begin();

    /* start soft access point connections */
	EUtils.print("Setting soft-AP configuration ... ");
    EUtils.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    EUtils.print("Setting soft-AP ... ");
    String sAPName;
    String sAPPassword;
    if(EWifiControl.sHostName != "NOWIFI"){
        sAPName = "SB01" + EWifiControl.sHostName;
    }else{
        sAPName = "SB01" + EUtils.Get_MAC();
    }

    sAPPassword = EWifiControl.sHostPassword;
    EUtils.print("APName: ");
    EUtils.print(sAPName);
    EUtils.print("  APPassword: ");
    EUtils.print(sAPPassword);
    EUtils.print("  Status: ");
    EUtils.println(WiFi.softAP(sAPName,sAPPassword) ? "Ready" : "Failed!");
    EUtils.print("Soft-AP IP address = ");
    EUtils.println(WiFi.softAPIP());

    TaskRunner.init();
    EUtils.println("Initialized scheduler");

    TaskRunner.addTask(TestTask);
    EUtils.println("added TestTask");
    TestTask.enable();
    EUtils.println("Enabled TestTask");

    TaskRunner.addTask(WifiControlTask);
    EUtils.println("added WifiControlTask");
    WifiControlTask.enable();
    EUtils.println("Enabled WifiControlTask");

    TaskRunner.addTask(MqttControlTask);
    EUtils.println("added MqttControlTask");
    MqttControlTask.enable();
    EUtils.println("Enabled MqttControlTask");

    TaskRunner.addTask(MqttReconnectTask);

	/* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();

    lLastTime = millis();

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    connectToMqtt();

    MAlexa.Start();

    ERTC.Start();

    //ERTC.Update_Time_NTP();
}

void loop() {

    /* function to handle clients */
	ESPServer.handleClient();

    /* task manager */
    TaskRunner.execute();

    MAlexa.handleRequest();

    if(ui64OnemsTimeCounter > 1000){
        ui64OnemsTimeCounter = 0;
        ERTC.handleTiming();
        EApp.Check_RTC_TimeConfig();
        //Serial1.print("day: ");
        //Serial1.println(ERTC.daysOfTheWeek[ERTC.ui8DayOfWeek]);
        //ERTC.Get_DayOfWeek_Formula(ERTC.ui8DayOfMonth,ERTC.ui8Month,ERTC.ui8Year);
    }

	if(ui8DeviceResetFlag){
        delay(2000);
        ESP.restart();
    }

    yield();
}

/* This function sets 1 milisecond timer */
void Timer_1ms_Start(void){
    timer1_isr_init();
    timer1_attachInterrupt(timercallback1ms);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP); //80MHZ
    timer1_write(80000);//1ms
}

/* callback function 1ms interrupt */
void ICACHE_RAM_ATTR timercallback1ms(void){
    ui81msFlag=1;
    ui64OnemsTimeCounter++;
}

void TestCallback(void) {
    //Serial1.println("Working");
}   


void WifiControlV1Callback(void){
    String sString;
    
    //Mqtt_Check_For_Alexa_Sync();
    sString = EWifiControl.Checkout_Connection();
    if(EWifiControl.Get_Status() != WL_CONNECTED){
       Serial1.println("no wifi");
       digitalWrite(PIN_YELLOW_LED,!digitalRead(PIN_YELLOW_LED));
    }else{
        EApp.Application_Pin_Write(YELLOW_LED,HIGH);
    }
}

void InternetCallback(void){
    //EHttpClient.Check_ESP_Default_Ping(1000);
}

void MqttCallback (void){
    MQTT_Handle();
}

/* this function used to load all web server related files from spiffs */
bool ESPServer_LoadFromSpiffs(String path){
    String dataType = "text/plain";
    if(path.endsWith("/")) path += "index.htm";
    if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if(path.endsWith(".html")) dataType = "text/html";
    else if(path.endsWith(".htm")) dataType = "text/html";
    else if(path.endsWith(".css")) dataType = "text/css";
    else if(path.endsWith(".js")) dataType = "application/javascript";
    else if(path.endsWith(".png")) dataType = "image/png";
    else if(path.endsWith(".gif")) dataType = "image/gif";
    else if(path.endsWith(".jpg")) dataType = "image/jpeg";
    else if(path.endsWith(".ico")) dataType = "image/x-icon";
    else if(path.endsWith(".xml")) dataType = "text/xml";
    else if(path.endsWith(".pdf")) dataType = "application/pdf";
    else if(path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = SPIFFS.open(path.c_str(), "r");
    if (ESPServer.hasArg("download")) dataType = "application/octet-stream";
    if (ESPServer.streamFile(dataFile, dataType) != dataFile.size()) {}

    dataFile.close();
    return true;
}

void ESPServer_Mqtt_Values(void){
    String sData;
    StaticJsonDocument<1048> doc;

    doc["server"] = MqFile.sMqttServer; 
    doc["port"]   = String(MqFile.ui16MqttPort);
    doc["id"] = MqFile.sMqttClientId;
    doc["username"] = MqFile.sMqttUsername;
    doc["pass"] = MqFile.sMqttPassword;
    doc["keep"] = String(MqFile.ui16MqttKeepalive);
    doc["qos"] = String(MqFile.ui8MqttQoS);
    doc["relayP"] = MqFile.sMqttPubRelayTopic;
    doc["relayS"] = MqFile.sMqttSubRelayTopic;
    doc["rtcP"] = MqFile.sMqttPubRTCTopic;
    doc["rtcS"] = MqFile.sMqttSubRTCTopic;
    doc["alarmP"] = MqFile.sMqttPubAlarm0Topic;
    doc["alarmS"] = MqFile.sMqttSubAlarm0Topic;
    doc["tcP"] = MqFile.sMqttPubTCTopic;
    doc["tcS"] = MqFile.sMqttSubTCTopic;

    serializeJson(doc, sData);  

    ESPServer.send(200, "text/json",sData);
}   

void ESPServer_Alexa_Values(void){
    String sData;
    StaticJsonDocument<1048> doc;

    doc["token"] = MAlexa.sAlexaAPIKey;
    doc["id"]   = MAlexa.sSBRelayId;
    doc["name"] = MAlexa.sSBRelayName;
    serializeJson(doc, sData);  

    ESPServer.send(200, "text/json",sData);
}

void ESPServer_Wifi_Values(void){
    String sData;
    StaticJsonDocument<2048> doc;
    long lRSSI;

    // Measure Signal Strength (RSSI) of Wi-Fi connection
    lRSSI = WiFi.RSSI();
    if(lRSSI <= -100){
        doc["rssi"] = 0;
    }else if(lRSSI >= -50){
        doc["rssi"] = 100;
    }else{
        doc["rssi"] = 2 * (lRSSI + 100);
    }

    doc["MAC"] = EUtils.Get_MAC();

    if(EWifiControl.Get_Status() != WL_CONNECTED){
        doc["WifiStatus"] = "ERROR";
    }else{
        doc["WifiStatus"] = "CONNECTED";
    }
    
    doc["CSSID"] = WiFi.SSID();
    doc["CIP"] = WiFi.localIP().toString();
    doc["CGW"] = WiFi.gatewayIP().toString();
    doc["CSN"] = WiFi.subnetMask().toString();
    doc["CDNS"] = WiFi.dnsIP().toString();

    if(EWifiControl.sHostName == "NOWIFI"){
        doc["HOST Name"]  = "SB01" + EUtils.Get_MAC();
    }else{
        doc["HOST Name"]  = "SB01" + EWifiControl.sHostName;
    }    

    doc["Hostpassword"] = EWifiControl.sHostPassword;
    
    doc["AWN1"] = EWifiControl.sAddWifiName1;
    doc["AWP1"] = EWifiControl.sAddWifiPassword1;

    doc["AWN2"] = EWifiControl.sAddWifiName2;
    doc["AWP2"] = EWifiControl.sAddWifiPassword2;

    doc["AWN3"] = EWifiControl.sAddWifiName3;
    doc["AWP3"] = EWifiControl.sAddWifiPassword3;

    doc["AWN4"] = EWifiControl.sAddWifiName4;
    doc["AWP4"] = EWifiControl.sAddWifiPassword4;

    doc["IP1"] = EWifiControl.AddWifiIP1.toString();
    doc["GW1"] = EWifiControl.AddWifiGateway1.toString();
    doc["SN1"] = EWifiControl.AddWifiSubnet1.toString();
    doc["DN1"] = EWifiControl.AddWifiDNS1.toString();
    doc["CE1"] = EWifiControl.AddWifiConfig1.toString();
    doc["SE1"] = EWifiControl.ui8AddWifiConfigEnable1;

    doc["IP2"] = EWifiControl.AddWifiIP2.toString();
    doc["GW2"] = EWifiControl.AddWifiGateway2.toString();
    doc["SN2"] = EWifiControl.AddWifiSubnet2.toString();
    doc["DN2"] = EWifiControl.AddWifiDNS2.toString();
    doc["CE2"] = EWifiControl.AddWifiConfig2.toString();
    doc["SE2"] = EWifiControl.ui8AddWifiConfigEnable2;

    doc["IP3"] = EWifiControl.AddWifiIP3.toString();
    doc["GW3"] = EWifiControl.AddWifiGateway3.toString();
    doc["SN3"] = EWifiControl.AddWifiSubnet3.toString();
    doc["DN3"] = EWifiControl.AddWifiDNS3.toString();
    doc["CE3"] = EWifiControl.AddWifiConfig3.toString();
    doc["SE3"] = EWifiControl.ui8AddWifiConfigEnable3;

    doc["IP4"] = EWifiControl.AddWifiIP4.toString();
    doc["GW4"] = EWifiControl.AddWifiGateway4.toString();
    doc["SN4"] = EWifiControl.AddWifiSubnet4.toString();
    doc["DN4"] = EWifiControl.AddWifiDNS4.toString();
    doc["CE4"] = EWifiControl.AddWifiConfig4.toString();
    doc["SE4"] = EWifiControl.ui8AddWifiConfigEnable4;

    serializeJson(doc, sData);

    ESPServer.send(200, "text/json",sData);
}

/* this function updates the modbus on client request */
void ESPServer_SwicthBinaryUpdate(void){
   	String sData;
    StaticJsonDocument<2048> doc;
        
    /* Measure Signal Strength (RSSI) of Wi-Fi connection */
    if(WiFi.RSSI() <= -100){
        doc["rssi"] = 0;
    }else if(WiFi.RSSI() >= -50){
        doc["rssi"] = 100;
    }else{
        doc["rssi"] = 2 * (WiFi.RSSI() + 100);
    }

    doc["ledg"] = EApp.Application_Pin_Read(GREEN_LED);
    doc["ledy"] = EApp.Application_Pin_Read(YELLOW_LED);
    doc["alexaS"] = EApp.ui8AlexaStatus;
    doc["mqttS"] = EApp.ui8MqttStatus;
	if(EApp.Application_Pin_Read(RELAY)){
		doc["relayS"] = "Relay On";
	}else{
		doc["relayS"] = "Relay Off";
	}
    doc["MAC"] = WiFi.macAddress();
    
    doc["time"] = ERTC.Get_DateTime_String();
    doc["S0"] = EApp.Get_Timeconfig(0,0);
    doc["E0"] = EApp.Get_Timeconfig(0,1);
    doc["S1"] = EApp.Get_Timeconfig(1,0);
    doc["E1"] = EApp.Get_Timeconfig(1,1);
    doc["S2"] = EApp.Get_Timeconfig(2,0);
    doc["E2"] = EApp.Get_Timeconfig(2,1);
    doc["S3"] = EApp.Get_Timeconfig(3,0);
    doc["E3"] = EApp.Get_Timeconfig(3,1);
    doc["S4"] = EApp.Get_Timeconfig(4,0);
    doc["E4"] = EApp.Get_Timeconfig(4,1);
    doc["S5"] = EApp.Get_Timeconfig(5,0);
    doc["E5"] = EApp.Get_Timeconfig(5,1);
    doc["S6"] = EApp.Get_Timeconfig(6,0);
    doc["E6"] = EApp.Get_Timeconfig(6,1);
    doc["S7"] = EApp.Get_Timeconfig(7,0);
    doc["E7"] = EApp.Get_Timeconfig(7,1);
    doc["S8"] = EApp.Get_Timeconfig(8,0);
    doc["E8"] = EApp.Get_Timeconfig(8,1);
    doc["S9"] = EApp.Get_Timeconfig(9,0);
    doc["E9"] = EApp.Get_Timeconfig(9,1);
    doc["S10"] = EApp.Get_Timeconfig(10,0);
    doc["E10"] = EApp.Get_Timeconfig(10,1);
    doc["S11"] = EApp.Get_Timeconfig(11,0);
    doc["E11"] = EApp.Get_Timeconfig(11,1);
    doc["S12"] = EApp.Get_Timeconfig(12,0);
    doc["E12"] = EApp.Get_Timeconfig(12,1);
    doc["S13"] = EApp.Get_Timeconfig(13,0);
    doc["E13"] = EApp.Get_Timeconfig(13,1);
    doc["S14"] = EApp.Get_Timeconfig(14,0);
    doc["E14"] = EApp.Get_Timeconfig(14,1);
    doc["S15"] = EApp.Get_Timeconfig(15,0);
    doc["E15"] = EApp.Get_Timeconfig(15,1);
    doc["A0"] = String(EApp.aui8TimeConfig[(0 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A1"] = String(EApp.aui8TimeConfig[(1 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A2"] = String(EApp.aui8TimeConfig[(2 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A3"] = String(EApp.aui8TimeConfig[(3 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A4"] = String(EApp.aui8TimeConfig[(4 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A5"] = String(EApp.aui8TimeConfig[(5 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A6"] = String(EApp.aui8TimeConfig[(6 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A7"] = String(EApp.aui8TimeConfig[(7 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A8"] = String(EApp.aui8TimeConfig[(8 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A9"] = String(EApp.aui8TimeConfig[(9 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A10"] = String(EApp.aui8TimeConfig[(10 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A11"] = String(EApp.aui8TimeConfig[(11 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A12"] = String(EApp.aui8TimeConfig[(12 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A13"] = String(EApp.aui8TimeConfig[(13 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A14"] = String(EApp.aui8TimeConfig[(14 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["A15"] = String(EApp.aui8TimeConfig[(15 * 8) + MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    doc["TC"] = String(EApp.ui8RTCTimeConfigStatus);
    doc["OS"] = EApp.Get_OTTimeconfig(0);
    doc["OE"] = EApp.Get_OTTimeconfig(1);
    doc["OA"] = String(EApp.aui8OTTimeConfig[MEMORY_TIMECONFIG_ACTIVE_INDEX]);
    serializeJson(doc, sData);  
    
    ESPServer.send(200, "text/json",sData);
}

/* This handling root file */
void ESPServer_HandleRoot(void){
  ESPServer.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  ESPServer.send(302, "text/plane","");
}

/* this function handle when wrong url path requented */
void ESPServer_HandleWebRequests(void){
    if(ESPServer_LoadFromSpiffs(ESPServer.uri())) return;
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += ESPServer.uri();
    message += "\nMethod: ";
    message += (ESPServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += ESPServer.args();
    message += "\n";
    for (uint8_t i=0; i<ESPServer.args(); i++){
        message += " NAME:"+ESPServer.argName(i) + "\n VALUE:" + ESPServer.arg(i) + "\n";
    }

    ESPServer.send(404, "text/plain", message);
}

void ESPServer_Handle_Alexa(void){
    String sAlexaAPI;
    String sSBRelayID;
    String sSBRelayName;

    String  sMsg = " ";
    
    sAlexaAPI   = ESPServer.arg("sAlexaAPI");
    sSBRelayID = ESPServer.arg("sAlexarelayid");
    sSBRelayName = ESPServer.arg("sAlexarelayname");

    EUtils.println(sAlexaAPI);
    EUtils.println(sSBRelayID);
    EUtils.println(sSBRelayName);
     
   if(sAlexaAPI.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[ALEXA_TOKEN_INDEX],&EFile.aui32ESPFileSize[ALEXA_TOKEN_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[ALEXA_TOKEN_INDEX],sAlexaAPI) == FILE_STORE_SUCCESS){
                sMsg = "Alexa API keys store to file sucess";
                MAlexa.Update_Specified_File(ALEXA_TOKEN_INDEX);
            }else{
                sMsg = "Alexa API keys store to file fail";
            }
       }else{
           sMsg = "Fail to delete Alexa API file";
       }
       
       //EUtils.println("File read"); 
       //EUtils.println(EFile.Read_Data_From_File(sESPFiles[ALEXA_TOKEN_INDEX]));
    }

    if(sSBRelayID.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[ALEXA_SWBIN_ID_INDEX],&EFile.aui32ESPFileSize[ALEXA_SWBIN_ID_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[ALEXA_SWBIN_ID_INDEX],sSBRelayID) == FILE_STORE_SUCCESS){
                sMsg = sMsg + " sSBRelayID store to file sucess";
                MAlexa.Update_Specified_File(ALEXA_SWBIN_ID_INDEX);
            }else{
                sMsg = sMsg + " sSBRelayID keys store to file fail";
            }
       }else{
           sMsg = sMsg + " Fail to delete sSBRelayID file";
       }
       
       //EUtils.println("File read"); 
       //EUtils.println(EFile.Read_Data_From_File(sESPFiles[sACInput1ID]));
    }

    if(sSBRelayName.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[ALEXA_SWBIN_NAME_INDEX],&EFile.aui32ESPFileSize[ALEXA_SWBIN_NAME_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[ALEXA_SWBIN_NAME_INDEX],sSBRelayName) == FILE_STORE_SUCCESS){
                sMsg = sMsg + " sSBRelayName store to file sucess";
                MAlexa.Update_Specified_File(ALEXA_SWBIN_NAME_INDEX);
            }else{
                sMsg = sMsg + " sSBRelayName keys store to file fail";
            }
       }else{
           sMsg = sMsg + " Fail to delete sSBRelayName file";
       }
       
       //EUtils.println("File read"); 
       //EUtils.println(EFile.Read_Data_From_File(sESPFiles[sACInput1ID]));
    }

    if(sMsg.equals(" ")){
        sMsg = "no action taken ";
    }

    EUtils.println(sMsg);

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_MqttParameter (void){
    String sMqttServer;
	String sMqttPort;
	String sMqttClientId;
	String sMqttUsername;
	String sMqttPassword;
	String sMqttRelayTopic;
    String sMqttAlarmTopic;
    String sMqttRTCTopic;
    String sMqttTCTopic;
    String sQos;
    String sKeepalive;
    String  sMsg = " ";
    
    sMqttServer             = ESPServer.arg("sMqttServer");
    sMqttPort               = ESPServer.arg("sMqttPort");
    sMqttClientId           = ESPServer.arg("sMqttClientId");
    sMqttUsername           = ESPServer.arg("sMqttUsername");
    sMqttPassword           = ESPServer.arg("sMqttPassword");
    sMqttRelayTopic         = ESPServer.arg("sMqttRelayTopic");
    sMqttAlarmTopic         = ESPServer.arg("sMqttAlarmTopic");
    sMqttRTCTopic           = ESPServer.arg("sMqttRTCTopic");
    sMqttTCTopic            = ESPServer.arg("sMqttTCTopic");
    sQos                    = ESPServer.arg("sQos");
    sKeepalive              = ESPServer.arg("sMqttKeepalive");
    
    EUtils.println(sMqttServer);
    EUtils.println(sMqttPort);
    EUtils.println(sMqttClientId);
    EUtils.println(sMqttUsername);
    EUtils.println(sMqttPassword);
    EUtils.println(sMqttRelayTopic);
    EUtils.println(sMqttRTCTopic);
    EUtils.println(sQos);
    EUtils.println(sKeepalive);
    
    if(sMqttServer.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_SERVER_INDEX],&EFile.aui32ESPFileSize[MQTT_SERVER_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_SERVER_INDEX],sMqttServer) == FILE_STORE_SUCCESS){
                sMsg = "mqtt server to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_SERVER_INDEX);
            }else{
                sMsg = "mqtt server store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt server file";
       }
    }

    if(sMqttPort.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PORT_INDEX],&EFile.aui32ESPFileSize[MQTT_PORT_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PORT_INDEX],sMqttPort) == FILE_STORE_SUCCESS){
                sMsg = "mqtt port to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PORT_INDEX);
            }else{
                sMsg = "mqtt port store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt port file";
       }
    }

    if(sMqttClientId.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_CLIENTID_INDEX],&EFile.aui32ESPFileSize[MQTT_CLIENTID_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_CLIENTID_INDEX],sMqttClientId) == FILE_STORE_SUCCESS){
                sMsg = "mqtt clientid to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_CLIENTID_INDEX);
            }else{
                sMsg = "mqtt clientid store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt clientid file";
       }
    }

    if(sMqttUsername.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_USERNAME_INDEX],&EFile.aui32ESPFileSize[MQTT_USERNAME_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_USERNAME_INDEX],sMqttUsername) == FILE_STORE_SUCCESS){
                sMsg = "mqtt username to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_USERNAME_INDEX);
            }else{
                sMsg = "mqtt username store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt username file";
       }
    }


    if(sMqttPassword.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PASSWORD_INDEX],&EFile.aui32ESPFileSize[MQTT_PASSWORD_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PASSWORD_INDEX],sMqttPassword) == FILE_STORE_SUCCESS){
                sMsg = "mqtt password to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PASSWORD_INDEX);
            }else{
                sMsg = "mqtt password store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt password file";
       }
    }

    if(sMqttRelayTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PUB_RELAY_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_PUB_RELAY_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PUB_RELAY_TOPIC_INDEX],sMqttRelayTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttRelayTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PUB_RELAY_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttRelayTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttRelayTopic file";
       }
    }

    if(sMqttRelayTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_SUB_RELAY_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_SUB_RELAY_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_SUB_RELAY_TOPIC_INDEX],sMqttRelayTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttRelayTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_SUB_RELAY_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttRelayTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttRelayTopic file";
       }
    }

    if(sMqttRTCTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PUB_RTC_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_PUB_RTC_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PUB_RTC_TOPIC_INDEX],sMqttRTCTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttRTCTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PUB_RTC_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttRTCTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttRTCTopic file";
       }
    }

    if(sMqttRTCTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_SUB_RTC_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_SUB_RTC_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_SUB_RTC_TOPIC_INDEX],sMqttRTCTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttRTCTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_SUB_RTC_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttRTCTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttRTCTopic file";
       }
    }

    if((sQos.equals("")== false) and (sQos.equals("4")== false)){
        
       if(EFile.DeleteFile(sESPFiles[MQTT_QOS_INDEX],&EFile.aui32ESPFileSize[MQTT_QOS_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_QOS_INDEX],sQos) == FILE_STORE_SUCCESS){
                sMsg = "mqtt qos to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_QOS_INDEX);
            }else{
                sMsg = "mqtt qos store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt qos file";
       }
    }


    if(sKeepalive.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_KEEPALIVE_INDEX],&EFile.aui32ESPFileSize[MQTT_KEEPALIVE_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_KEEPALIVE_INDEX],sKeepalive) == FILE_STORE_SUCCESS){
                sMsg = "mqtt keepalive to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_KEEPALIVE_INDEX);
            }else{
                sMsg = "mqtt keepalive store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt keepalive file";
       }
    }

    if(sMqttTCTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PUB_TC_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_PUB_TC_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PUB_TC_TOPIC_INDEX],sMqttTCTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttTCTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PUB_TC_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttTCTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttTCTopic file";
       }
    }

    if(sMqttTCTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_SUB_TC_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_SUB_TC_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_SUB_TC_TOPIC_INDEX],sMqttTCTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttTCTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_SUB_TC_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttTCTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttTCTopic file";
       }
    }


    if(sMqttAlarmTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_PUB_ALARM_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_PUB_ALARM_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_PUB_ALARM_TOPIC_INDEX],sMqttAlarmTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttAlarmTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_PUB_ALARM_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttAlarmTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttAlarmTopic file";
       }
    }

    if(sMqttAlarmTopic.equals("")== false){

       if(EFile.DeleteFile(sESPFiles[MQTT_SUB_ALARM_TOPIC_INDEX],&EFile.aui32ESPFileSize[MQTT_SUB_ALARM_TOPIC_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[MQTT_SUB_ALARM_TOPIC_INDEX],sMqttAlarmTopic) == FILE_STORE_SUCCESS){
                sMsg = "mqtt sMqttAlarmTopic to file sucess";
                MqFile.Read_Mqtt_Parameter(MQTT_SUB_ALARM_TOPIC_INDEX);
            }else{
                sMsg = "mqtt sMqttAlarmTopic store to file fail";
            }
       }else{
           sMsg = "Fail to delete mqtt sMqttAlarmTopic file";
       }
    }

    if(sMsg.equals(" ")){
        sMsg = "no action taken ";
    }

    EUtils.println(sMsg);

    MqFile.Read_All_Mqtt_Parameters();
    
    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_WifiAdd(void){
    String sWifiName;
    String sWifiPassword;
    uint8_t ui8WifiNumber;
    String  sMsg;

    if (ESPServer.arg("username")!= ""){
        sWifiName = ESPServer.arg("username");
        if (ESPServer.arg("password")!= ""){
            sWifiPassword = ESPServer.arg("password");
        }else{
            sWifiPassword = "NoPassword";
        }

        if (ESPServer.arg("number")!= ""){
            ui8WifiNumber = ESPServer.arg("number").toInt();
            if(ui8WifiNumber > 3){
                ui8WifiNumber = 3;
            }

            if(ui8WifiNumber < 0){
                ui8WifiNumber = 0;
            }

        }else{
            ui8WifiNumber = 0;
        }

        if(EFile.DeleteFile(sESPFiles[WIFI_1_SSID_INDEX + ui8WifiNumber],&EFile.aui32ESPFileSize[WIFI_1_SSID_INDEX + ui8WifiNumber]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[WIFI_1_SSID_INDEX+ ui8WifiNumber],sWifiName) == FILE_STORE_SUCCESS){
                sMsg = "Add wifi sucess";
                if(EFile.DeleteFile(sESPFiles[WIFI_1_PASSWORD_INDEX + ui8WifiNumber],&EFile.aui32ESPFileSize[WIFI_1_PASSWORD_INDEX + ui8WifiNumber]) == FILE_DELETE_SUCCESS){
                    if(EFile.Write_Data_To_File(sESPFiles[WIFI_1_PASSWORD_INDEX + ui8WifiNumber],sWifiPassword) == FILE_STORE_SUCCESS){
                        sMsg = "Add wifi sucess";
                        EWifiControl.Update_Wifi_Add_File(ui8WifiNumber);
                    }else{
                        sMsg = "Add wifi fail";
                    }
                }else{
               
                    sMsg = "Fail to delete file";
                }
            }else{
                sMsg = "Add wifi fail";
            }
        }else{
           sMsg = "Fail to delete file";
        }
    }

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_WifiHostname(void){
    String sMsg;

    if (ESPServer.arg("hostname")!= ""){
        if(EFile.DeleteFile(sESPFiles[WIFI_HOSTNAME_INDEX],&EFile.aui32ESPFileSize[WIFI_HOSTNAME_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[WIFI_HOSTNAME_INDEX],ESPServer.arg("hostname")) == FILE_STORE_SUCCESS){
                sMsg = "wifi hostname received";
            }else{
                sMsg = "wifi hostname failed";
            }
        }else{
           sMsg = "Fail to delete file";
        }
    }

    if (ESPServer.arg("hostpassword")!= ""){
        if(EFile.DeleteFile(sESPFiles[HOST_PASSWORD_INDEX],&EFile.aui32ESPFileSize[HOST_PASSWORD_INDEX]) == FILE_DELETE_SUCCESS){
            if(EFile.Write_Data_To_File(sESPFiles[HOST_PASSWORD_INDEX],ESPServer.arg("hostpassword")) == FILE_STORE_SUCCESS){
                sMsg = "wifi hostpassword received";
            }else{
                sMsg = "wifi hostpassword failed";
            }
        }else{
           sMsg = "Fail to delete file";
        }
    }

    EWifiControl.Update_Host_Name();

    ESPServer.send(200, "text/plain",sMsg);
}


void ESPServer_Handle_WifiParameterAdd(void){
    String sIpAddr;
    String sGateway;
    String sSubnet;
    String sDNSIP;
    String sConfigEnable;
    String sMsg;
    String sWifiParamter;
    String sSubString;
    uint8_t ui8WifiIndex;
    
    sIpAddr = ESPServer.arg("ipaddr");
    sGateway = ESPServer.arg("gateway");
    sSubnet = ESPServer.arg("subnet");
    sDNSIP = ESPServer.arg("dnsip");
    sConfigEnable = ESPServer.arg("staticconfig");
    ui8WifiIndex  = ESPServer.arg("wifinumber").toInt();

    Serial1.println("file index");
    Serial1.println(ui8WifiIndex);

    if(sIpAddr.equals("A.B.C.D") == false){
        if(EWifiControl.Update_Wifi_Add_Static_Config(ui8WifiIndex,2,sIpAddr)){
            EWifiControl.Get_Wifi_Add_Parameter(ui8WifiIndex);
            sMsg  = "Wifi IP add sucess";
        }else{
            sMsg  = "Wifi IP add failed";
        }
    }

    if(sGateway.equals("A.B.C.D") == false){
        if(EWifiControl.Update_Wifi_Add_Static_Config(ui8WifiIndex,3,sGateway)){
            EWifiControl.Get_Wifi_Add_Parameter(ui8WifiIndex);
            sMsg  = "Wifi Gateway add sucess";
        }else{
            sMsg  = "Wifi Gateway add failed";
        }
    }
    
    if(sSubnet.equals("A.B.C.D") == false){
        if(EWifiControl.Update_Wifi_Add_Static_Config(ui8WifiIndex,4,sSubnet)){
            EWifiControl.Get_Wifi_Add_Parameter(ui8WifiIndex);
            sMsg  = "Wifi add sucess";
        }else{
            sMsg  = "Wifi add failed";
        }
    }

    if(sDNSIP.equals("A.B.C.D") == false){
        if(EWifiControl.Update_Wifi_Add_Static_Config(ui8WifiIndex,5,sDNSIP)){
            EWifiControl.Get_Wifi_Add_Parameter(ui8WifiIndex);
            sMsg  = "Wifi add sucess";
        }else{
            sMsg  = "Wifi add failed";
        }
    }

    if(sConfigEnable.equals("2") == false){
        if(EWifiControl.Update_Wifi_Add_Static_Config(ui8WifiIndex,1,sConfigEnable)){
            EWifiControl.Get_Wifi_Add_Parameter(ui8WifiIndex);
            sMsg  = "Wifi add sucess";
        }else{
            sMsg  = "Wifi add failed";
        }
    }

    ESPServer.send(200, "text/plain",sMsg);
    
}

void MQTT_Handle(void) {
    if(ui8MqttSendFlag){
        Mqtt_Publish_All();
    }
}

void onMqttPublish(uint16_t packetId) {
  Serial1.println("Publish acknowledged.");
  
  //Serial1.print("  packetId: ");
  //Serial1.println(packetId);
  //sEndTime = micros();
  //Serial1.println(sEndTime - sStratTime);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  uint8_t ui8Payload;

  Serial1.println("Publish received.");
  Serial1.print("  topic: ");
  Serial1.println(topic);
  Serial1.print("  qos: ");
  Serial1.println(properties.qos);
  Serial1.print("  dup: ");
  Serial1.println(properties.dup);
  Serial1.print("  retain: ");
  Serial1.println(properties.retain);
  Serial1.print("  len: ");
  Serial1.println(len);
  Serial1.print("  index: ");
  Serial1.println(index);
  Serial1.print("  total: ");
  Serial1.println(total);
  Serial1.println(payload);

    ui8Payload = String(payload).toInt();
    Serial1.print("Payload: ");
    Serial1.println(ui8Payload);

    if(strcmp(topic,MqFile.sMqttSubRelayTopic.c_str()) == 0){
        if(ui8Payload){
            EApp.Application_Pin_Write(RELAY, HIGH);
        }else{
            EApp.Application_Pin_Write(RELAY, LOW);
        }
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubRTCTopic.c_str()) == 0){
        if(ui8Payload){
            ERTC.Update_Time_NTP();
        }

        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubTCTopic.c_str()) == 0){
        if(ui8Payload){
            EEPROM.write(MEMORY_RTC_TIMECONFIG_BYTE_INDEX,1);
            EEPROM.commit();
            EMemory.Update_All();
        }else{
            EEPROM.write(MEMORY_RTC_TIMECONFIG_BYTE_INDEX,0);
            EEPROM.commit();
            EMemory.Update_All();
        }
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm0Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(0,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm1Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(1,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm2Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(2,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm3Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(3,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm4Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(4,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm5Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(5,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm6Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(6,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm7Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(7,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm8Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(8,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm9Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(9,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm10Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(10,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm11Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(11,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm12Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(12,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm13Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(13,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm14Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(14,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubAlarm15Topic.c_str()) == 0){
        EApp.Set_Timeconfig_Complete(15,String(payload));
        Mqtt_Publish_All();
    }

    if(strcmp(topic,MqFile.sMqttSubOTAlarmTopic.c_str()) == 0){
        EApp.Set_OTTimeconfig_Complete(String(payload));
        Mqtt_Publish_All();
    }
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial1.println("Unsubscribe acknowledged.");
  Serial1.print("  packetId: ");
  Serial1.println(packetId);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial1.println("Subscribe acknowledged.");
  Serial1.print("  packetId: ");
  Serial1.println(packetId);
  Serial1.print("  qos: ");
  Serial1.println(qos);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial1.print("Disconnected from MQTT Reason: ");
    Serial1.printf("%d",reason);
    Serial1.println();
    
    ui8MqttSendFlag = 0;
    EApp.ui8MqttStatus = 0;

    if (WiFi.isConnected()) {
        connectToMqtt();
        //MqttReconnectTask.enable();
    }
}

void connectToMqtt(void) {
  Serial1.println("Connecting to MQTT...");
  mqttClient.setClientId(MqFile.sMqttClientId.c_str());
  mqttClient.setCredentials(MqFile.sMqttUsername.c_str(),MqFile.sMqttPassword.c_str());
  mqttClient.setServer(MqFile.sMqttServer.c_str(),MqFile.ui16MqttPort);
  mqttClient.setKeepAlive(MqFile.ui16MqttKeepalive);
  mqttClient.setWill(MqFile.sMqttDeviceStatus.c_str(),MqFile.ui8MqttQoS,1,"offline",strlen("offline"));
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
    
    Serial1.println("Connected to MQTT.");
    Serial1.print("Session present: ");
    Serial1.println(sessionPresent);

    ui8MqttSendFlag = 1;
    EApp.ui8MqttStatus = 1;

    mqttClient.subscribe(MqFile.sMqttSubRelayTopic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubRTCTopic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubTCTopic.c_str(),MqFile.ui8MqttQoS);
    
    mqttClient.subscribe(MqFile.sMqttSubAlarm0Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm1Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm2Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm3Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm4Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm5Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm6Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm7Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm8Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm9Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm10Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm11Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm12Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm13Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm14Topic.c_str(),MqFile.ui8MqttQoS);
    mqttClient.subscribe(MqFile.sMqttSubAlarm15Topic.c_str(),MqFile.ui8MqttQoS);

    mqttClient.subscribe(MqFile.sMqttSubOTAlarmTopic.c_str(),MqFile.ui8MqttQoS);

    Mqtt_Publish_All();
    mqttClient.publish(MqFile.sMqttDeviceStatus.c_str(), MqFile.ui8MqttQoS, true,"online");
}

void Mqtt_Publish_All(void){

    mqttClient.publish(MqFile.sMqttPubRelayTopic.c_str(),MqFile.ui8MqttQoS, true,String(EApp.ui8RelayStatus).c_str());

    mqttClient.publish(MqFile.sMqttPubRTCTopic.c_str(),MqFile.ui8MqttQoS, true,ERTC.Get_DateTime_String().c_str());

    mqttClient.publish(MqFile.sMqttPubTCTopic.c_str(),MqFile.ui8MqttQoS, true,String(EApp.ui8RTCTimeConfigStatus).c_str());

    mqttClient.publish(MqFile.sMqttPubAlarm0Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(0).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm1Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(1).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm2Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(2).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm3Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(3).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm4Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(4).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm5Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(5).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm6Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(6).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm7Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(7).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm8Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(8).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm9Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(9).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm10Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(10).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm11Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(11).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm12Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(12).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm13Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(13).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm14Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(14).c_str());
    mqttClient.publish(MqFile.sMqttPubAlarm15Topic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_Timeconfig_Complete(15).c_str());

    mqttClient.publish(MqFile.sMqttPubOTAlarmTopic.c_str(),MqFile.ui8MqttQoS, true,EApp.Get_OTTimeconfig_Complete().c_str());

    Serial1.println("Publish relay status");
}

void Mqtt_Check_For_Alexa_Sync(void){
    if(EApp.ui8AlexaUpdated){
        EUtils.println("Alexa mqtt sync");
        EApp.ui8AlexaUpdated=0;
        MQTT_Handle();
    }
}

/* this function updates the relay status on client request */
void ESPServer_RelayUpdate(void){
   	String sData;
	
  	Serial1.print("Relay web request: ");
  	Serial1.println(ESPServer.arg("state"));
	if (ESPServer.arg("state")=="1"){
		sData = "Relay:on";
		EApp.Application_Pin_Write(RELAY, HIGH);
	}else{
		sData = "Relay:off";
		EApp.Application_Pin_Write(RELAY, LOW);
	}
	Serial1.println(sData);
	ESPServer.send(200, "text/json",sData);
}

void ESPServer_RTCUpdate(void){
    String sReset;
   
    String  sMsg = " ";
    
    sReset   = ESPServer.arg("value");
    
    if(sReset.equals("1")){
        sMsg = "rtc updates now";
        ERTC.Update_Time_NTP();
    }

    if(sMsg.equals(" ")){
        sMsg = "no action taken ";
    }

    EUtils.println(sMsg);

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_DeviceReset (void){
    String sReset;
   
    String  sMsg = " ";
    
    sReset   = ESPServer.arg("value");
    
    if(sReset.equals("1")){
        sMsg = "device will resetart now";
        ui8DeviceResetFlag=1;
    }

    if(sMsg.equals(" ")){
        sMsg = "no action taken ";
    }

    EUtils.println(sMsg);

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Enable_Timeconfig (void){
    String sTC;
   
    String  sMsg = " Error in timeconfig enable";
    
    sTC   = ESPServer.arg("value");
    
    if(sTC.equals("1")){
        sMsg = "timeconfig enabled";
        EEPROM.write(MEMORY_RTC_TIMECONFIG_BYTE_INDEX,1);
        EEPROM.commit();
        EMemory.Update_All();
    }

    if(sTC.equals("0")){
        sMsg = "timeconfig disabled ";
        EEPROM.write(MEMORY_RTC_TIMECONFIG_BYTE_INDEX,0);
        EEPROM.commit();
        EMemory.Update_All();
    }

    EUtils.println(sMsg);

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_Timeconfig (void){
    String sReset;
    uint8_t ui8SHour;
    uint8_t ui8SMinute;
    uint8_t ui8SSecond;
    uint8_t ui8EHour;
    uint8_t ui8EMinute;
    uint8_t ui8ESecond;
    uint8_t ui8Enable;
    uint8_t ui8Index;
    uint8_t ui8Weekofday;

    String  sMsg = " ";
    
    ui8SHour   = ESPServer.arg("shour").toInt();
    ui8SMinute   = ESPServer.arg("sminute").toInt();
    ui8SSecond   = ESPServer.arg("ssecond").toInt();
    ui8EHour   = ESPServer.arg("ehour").toInt();
    ui8EMinute   = ESPServer.arg("eminute").toInt();
    ui8ESecond   = ESPServer.arg("esecond").toInt();
    ui8Enable   = ESPServer.arg("enable").toInt();
    ui8Index   = ESPServer.arg("index").toInt();
    ui8Weekofday = ESPServer.arg("sWeekday").toInt();

    Serial1.print("Weekofday: ");
    Serial1.println(ui8Weekofday);
    
    if((ui8Index < 16) && (ui8Enable < 2)){
        if((ui8SHour < 23) && (ui8SMinute < 60) && (ui8SSecond < 60)){
            if((ui8EHour < 23) && (ui8EMinute < 60) && (ui8ESecond < 60)){
                EMemory.Set_Timeconfig(ui8SHour,ui8SMinute,ui8SSecond,ui8EHour,ui8EMinute,ui8ESecond,ui8Index,ui8Enable,ui8Weekofday);
                sMsg = "timeconfig save sucess";            
            }else{
                sMsg = "Error in timeconfig";
            }    
        }else{
            sMsg = "Error in timeconfig";
        }    
    }else{
        sMsg = "Error in timeconfig";
    }

    ESPServer.send(200, "text/plain",sMsg);
}

void ESPServer_Handle_OTTimeconfig (void){
    String sReset;
    uint8_t ui8SHour;
    uint8_t ui8SMinute;
    uint8_t ui8SSecond;
    uint8_t ui8EHour;
    uint8_t ui8EMinute;
    uint8_t ui8ESecond;
    uint8_t ui8Enable;
    uint8_t ui8Weekofday;

    String  sMsg = " ";
    
    ui8SHour   = ESPServer.arg("shour").toInt();
    ui8SMinute   = ESPServer.arg("sminute").toInt();
    ui8SSecond   = ESPServer.arg("ssecond").toInt();
    ui8EHour   = ESPServer.arg("ehour").toInt();
    ui8EMinute   = ESPServer.arg("eminute").toInt();
    ui8ESecond   = ESPServer.arg("esecond").toInt();
    ui8Enable   = ESPServer.arg("enable").toInt();
    ui8Weekofday = ESPServer.arg("sWeekday").toInt();
    
    if(ui8Enable < 2){
        if((ui8SHour < 23) && (ui8SMinute < 60) && (ui8SSecond < 60)){
            if((ui8EHour < 23) && (ui8EMinute < 60) && (ui8ESecond < 60)){
                EMemory.Set_OTTimeconfig(ui8SHour,ui8SMinute,ui8SSecond,ui8EHour,ui8EMinute,ui8ESecond,ui8Enable,ui8Weekofday);
                sMsg = "OTtimeconfig save sucess";            
            }else{
                sMsg = "Error in OTtimeconfig";
            }    
        }else{
            sMsg = "Error in OTtimeconfig";
        }    
    }else{
        sMsg = "Error in OTtimeconfig";
    }

    ESPServer.send(200, "text/plain",sMsg);
}