#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// security
const String ACCESS_TOKEN = "sky";

// LCD api access
WiFiClient client;
const char* API_HOST = "192.168.22.7";
const int API_PORT = 8000;

// prototypes
boolean connectWifi();

// Change this before you flash
const char* ssid = "coldspot-DAF";
const char* password = "";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

// values
const bool ON = false;
const bool ON_OUTPUT = LOW;
const bool OFF = true;
const bool OFF_OUTPUT = HIGH;

// swtiches
Switch *control1 = NULL;
Switch *control2 = NULL;
Switch *control3 = NULL;
Switch *control4 = NULL;
Switch *control5 = NULL;
Switch *control6 = NULL;
Switch *control7 = NULL;
Switch *control8 = NULL;

// flags
bool isControl1On = false;
bool isControl2On = false;
bool isControl3On = false;
bool isControl4On = false;
bool isControl5On = false;
bool isControl6On = false;
bool isControl7On = false;
bool isControl8On = false;

// pin outs for controls
const int PIN_OUT_CONTROL_D0 = 16; // control 1
const int PIN_OUT_CONTROL_D1 = 5; // control 2
const int PIN_OUT_CONTROL_D2 = 4; // control 3
const int PIN_OUT_CONTROL_D3 = 0; // control 4
const int PIN_OUT_CONTROL_D4 = 2; // control 5
const int PIN_OUT_CONTROL_D5 = 14; // control 6
const int PIN_OUT_CONTROL_D6 = 12; // control 7
const int PIN_OUT_CONTROL_D7 = 13; // control 8

// web control server port
const int CONTROL_SERVER_PORT = 8000;
ESP8266WebServer *controlServer = NULL;

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){

    sendLcdDisplay("Setting up","controller..");
    
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    control1 = new Switch("Control One", 7000, control1On, control1Off);
    control2 = new Switch("Control Two", 7001, control2On, control2Off);
    control3 = new Switch("Control Three", 7002, control3On, control3Off);
    control4 = new Switch("Control Four", 7003, control4On, control4Off);
    control5 = new Switch("Control Five", 7004, control5On, control5Off);
    control6 = new Switch("Control Six", 7005, control6On, control6Off);
    control7 = new Switch("Control Seven", 7006, control7On, control7Off);
    control8 = new Switch("Control Eight", 7007, control8On, control8Off);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*control1);
    upnpBroadcastResponder.addDevice(*control2);
    upnpBroadcastResponder.addDevice(*control3);
    upnpBroadcastResponder.addDevice(*control4);
    upnpBroadcastResponder.addDevice(*control5);
    upnpBroadcastResponder.addDevice(*control6);
    upnpBroadcastResponder.addDevice(*control7);
    upnpBroadcastResponder.addDevice(*control8);

    // setup pins
    pinMode(PIN_OUT_CONTROL_D0, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D1, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D2, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D3, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D4, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D5, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D6, OUTPUT);
    pinMode(PIN_OUT_CONTROL_D7, OUTPUT);

    // turn of all
    sendLcdDisplay("Turning off","all switches");
    control1Off();
    control2Off();
    control3Off();
    control4Off();
    control5Off();
    control6Off();
    control7Off();
    control8Off();

    // setup control server
    sendLcdDisplay("Setup web server","switch control");
    setupControlServer();
    sendLcdDisplay("Switch controller","ready..");
    delay(2000);
    sendLcdDisplay("Getting swtich","statuses..");
    delay(2000);
    sendLcdDisplaySwitchStatuses();
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      control1->serverLoop();
      control2->serverLoop();
      control3->serverLoop();
      control4->serverLoop();
      control5->serverLoop();
      control6->serverLoop();
      control7->serverLoop();
      control8->serverLoop();

      if(controlServer != NULL) {
        controlServer->handleClient();
        delay(1);
      }
	 }
}

// control callback function definitions
bool control1On() {
    Serial.println("Switch Control 1 turn on ...");
    isControl1On = ON;
    digitalWrite(PIN_OUT_CONTROL_D0, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl1On;
}
bool control1Off() {
    Serial.println("Switch Control 1 turn off ...");
    isControl1On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D0, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl1On;
}
bool control2On() {
    Serial.println("Switch Control 2 turn on ...");
    isControl2On = ON;
    digitalWrite(PIN_OUT_CONTROL_D1, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl2On;
}
bool control2Off() {
    Serial.println("Switch Control 2 turn off ...");
    isControl2On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D1, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl2On;
}
bool control3On() {
    Serial.println("Switch Control 3 turn on ...");
    isControl3On = ON;
    digitalWrite(PIN_OUT_CONTROL_D2, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl3On;
}
bool control3Off() {
    Serial.println("Switch Control 3 turn off ...");
    isControl3On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D2, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl3On;
}
bool control4On() {
    Serial.println("Switch Control 4 turn on ...");
    isControl4On = ON;
    digitalWrite(PIN_OUT_CONTROL_D3, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl4On;
}
bool control4Off() {
    Serial.println("Switch Control 4 turn off ...");
    isControl4On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D3, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl4On;
}
bool control5On() {
    Serial.println("Switch Control 5 turn on ...");
    isControl5On = ON;
    digitalWrite(PIN_OUT_CONTROL_D4, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl5On;
}
bool control5Off() {
    Serial.println("Switch Control 5 turn off ...");
    isControl5On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D4, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl5On;
}
bool control6On() {
    Serial.println("Switch Control 6 turn on ...");
    isControl6On = ON;
    digitalWrite(PIN_OUT_CONTROL_D5, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl6On;
}
bool control6Off() {
    Serial.println("Switch Control 6 turn off ...");
    isControl6On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D5, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl6On;
}
bool control7On() {
    Serial.println("Switch Control 7 turn on ...");
    isControl7On = ON;
    digitalWrite(PIN_OUT_CONTROL_D6, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl7On;
}
bool control7Off() {
    Serial.println("Switch Control 7 turn off ...");
    isControl7On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D6, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl7On;
}
bool control8On() {
    Serial.println("Switch Control 8 turn on ...");
    isControl8On = ON;
    digitalWrite(PIN_OUT_CONTROL_D7, ON_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl8On;
}
bool control8Off() {
    Serial.println("Switch Control 8 turn off ...");
    isControl8On = OFF;
    digitalWrite(PIN_OUT_CONTROL_D7, OFF_OUTPUT);
    sendLcdDisplaySwitchStatuses();
    return isControl8On;
}

// handle web controls
void setupControlServer() {

  controlServer = new ESP8266WebServer(CONTROL_SERVER_PORT);

  controlServer->on("/", [&]() {
    handleWebControls();
  });
  controlServer->on("/status", [&]() {
    handleGetSwitchesStatus();
  });

  controlServer->begin();

  Serial.println("Control Server started on port: ");
  Serial.println(CONTROL_SERVER_PORT);
}

// handle switching controls via web api
void handleWebControls() {
  
    String argName1 = controlServer->argName(0); // access token
    String argName2 = controlServer->argName(1); // switch number
    String argName3 = controlServer->argName(2); // switch value
    
    String token = controlServer->arg(0);

    if(token != ACCESS_TOKEN) {
      controlServer->send(401, "text/plain", "access denied!");
      return;
    }

    if(argName2 == "control" && argName3 == "value") {

      String controlSwitch = controlServer->arg(1);
      String state = controlServer->arg(2);
  
      if(controlSwitch == "1") (state == "1") ? control1On() : control1Off();
      if(controlSwitch == "2") (state == "1") ? control2On() : control2Off();
      if(controlSwitch == "3") (state == "1") ? control3On() : control3Off();
      if(controlSwitch == "4") (state == "1") ? control4On() : control4Off();
      if(controlSwitch == "5") (state == "1") ? control5On() : control5Off();
      if(controlSwitch == "6") (state == "1") ? control6On() : control6Off();
      if(controlSwitch == "7") (state == "1") ? control7On() : control7Off();
      if(controlSwitch == "8") (state == "1") ? control8On() : control8Off();
  
      controlServer->send(200, "text/plain", "ok");
      
    } else {
      controlServer->send(400, "text/plain", "Invalid Command");
    }
}

// handle getting switch statuses via web api 
void handleGetSwitchesStatus() {

    String response = "";

    response += (isControl1On == ON) ? "control1=1" : "control1=0";
    response += ",";
    response += (isControl2On == ON) ? "control2=1" : "control2=0";
    response += ",";
    response += (isControl3On == ON) ? "control3=1" : "control3=0";
    response += ",";
    response += (isControl4On == ON) ? "control4=1" : "control4=0";
    response += ",";
    response += (isControl5On == ON) ? "control5=1" : "control5=0";
    response += ",";
    response += (isControl6On == ON) ? "control6=1" : "control6=0";
    response += ",";
    response += (isControl7On == ON) ? "control7=1" : "control7=0";
    response += ",";
    response += (isControl8On == ON) ? "control8=1" : "control8=0";
    response += ",";
    response += "ip=" + WiFi.localIP();

    controlServer->send(200, "text/plain", response);
}

void sendLcdDisplay(String line1,String line2){

  // Use WiFiClient class to create TCP connections
  if (!client.connect(API_HOST, API_PORT)) {
    Serial.println("lcd web api connection failed");
    client.stop();
  } 
  else
  {
    Serial.println("lcd web api connected.");

    String url = "/?token=" + ACCESS_TOKEN;

    url += "&display1=" + urlencode(line1);
    url += "&display2=" + urlencode(line2);

    Serial.println("CMD: " + String(API_HOST) + url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + API_HOST + "\r\n" + 
                 "Connection: close\r\n\r\n");
    client.stop();
    Serial.println("\n[Disconnected]");
  }
}

void sendLcdDisplaySwitchStatuses() {
  
  String line1 = "1 2 3 4 5 6 7 8";
  String line2 = (isControl1On == ON) ? "ON" : "  ";
  line2 += (isControl2On == ON) ? "ON" : "  ";
  line2 += (isControl3On == ON) ? "ON" : "  ";
  line2 += (isControl4On == ON) ? "ON" : "  ";
  line2 += (isControl5On == ON) ? "ON" : "  ";
  line2 += (isControl6On == ON) ? "ON" : "  ";
  line2 += (isControl7On == ON) ? "ON" : "  ";
  line2 += (isControl8On == ON) ? "ON" : "  ";

  sendLcdDisplay(line1,line2);
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  
  boolean state = true;
  int i = 0;

  IPAddress ip(192, 168, 22, 20);
  IPAddress gateway(192, 168, 22, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

// helpers
// ====================================
String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
