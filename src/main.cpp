/*************************************************************
 *
 *
 * Author:  Thangvpv
 * Company: FPT Software
 * Descript: NodeMCU controling SIM800A project
 *
 *
 *************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include "SH1106.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>

// pin definitions for I2C
#define OLED_SDA  D1 // pin 1
#define OLED_SDC  D2 // pin 2
#define OLED_ADDR 0x3C

SH1106 display(OLED_ADDR, OLED_SDA, OLED_SDC);

// signal led
#define LED1 16
#define LED2 2

// display data, data structure
typedef struct DISPLAY_STRUCTURE {
  const char* displayTitle    = "Thangvpv";
  String displayIp       = "";
  String displayDetail   = "";
  String displayNotif    = "";
  String dispNotifStatus = ".....";
}DISPLAY_STRUCTURE;

typedef struct WEB_CONTENT {
  String pageRoot = "";
  String pageCall = "";
  String pageSend = "";
}WEB_CONTENT;

const String statusOK      = "OK";
const String statusNG      = "NG";
const String statusPending = ".....";

DISPLAY_STRUCTURE dispConfig;
WEB_CONTENT       pageLoader;

// wifi access point information
const char* ssid = "Dedsec_Access_Point";
const char* pass = "29011991";

// instant http port(80)
ESP8266WebServer server(80);

String page = "";

void callFunction();
void sendFunction();
void commandCrafter();
void sim800aTest();
void connectMode();
void codeDebugger();
void webContentInit();

void setup() {
    // start serial monitor
    Serial.begin(9600);
    delay(2000);
    Serial.println("begin initializing");

    // init oled display
    Serial.println("init oled display");
    display.init();
    while (!display.init()) {
      Serial.println("failed init display... retrying");
      display.displayOff();
      delay(1000);
      display.displayOn();
      display.clear();
      display.init();
    }
    Serial.println("done initializing display");
    codeDebugger();

    webContentInit();

    //make the LED pin output and initially turned off
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED2, LOW);
    delay(1000);


    // start access point
    dispConfig.displayNotif    = "starting access point";
    dispConfig.dispNotifStatus = statusPending;
    codeDebugger();
    /*
    size_t timeout = 0;
    while (!WiFi.softAP(ssid, pass)) {
      delay(500);
      Serial.print(".");
      display.drawProgressBar(4, 32, 120, 8, timeout);
      if (timeout == 100) {
        Serial.println("Access point initializing failed");
        Serial.println("Begin to connecting access points");
        dispConfig.dispNotifStatus = statusNG;
        codeDebugger();
        connectMode();
      }
      timeout++;
    }
    */
    connectMode();


    dispConfig.displayIp       = WiFi.localIP().toString();
    dispConfig.dispNotifStatus = statusOK;
    codeDebugger();

    Serial.println("");
    Serial.println("access point ready");


    // web service handler
    server.on("/", [](){
      server.send(200, "text/html", pageLoader.pageRoot);
    });

    // button call service
    server.on("/Call", [](){
      server.send(200, "text/html", pageLoader.pageRoot);
      callFunction();
      delay(1000);
    });

    // button send service
    server.on("/Send", [](){
      server.send(200, "text/html", pageLoader.pageRoot);
      sendFunction();
      delay(1000);
    });
    // start webserver
    server.begin();
    dispConfig.displayNotif    = "starting server";
    dispConfig.dispNotifStatus = statusPending;
    codeDebugger();
    server.begin();
    delay(2000);
    dispConfig.dispNotifStatus = statusOK;
    codeDebugger();

    Serial.println("web server started!");
    // test SIM800A module
    sim800aTest();
    // turn off led notif when done initializing
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    dispConfig.displayNotif    = "Running";
    //dispConfig.displayDetail   = WiFi.SSID();
    dispConfig.displayDetail = "Dedsec";
    dispConfig.dispNotifStatus = statusOK;
    codeDebugger();
    Serial.println("initializing done, NodeMCU ready to use");
}

void loop() {
    // put your main code here, to run repeatedly:
    // client connect handler
    server.handleClient();
}

void webContentInit() {
  // the HTML of the web page
  //page = "<h1>NodeMCU Web Server</h1><p> number <input type='text' name='phone_number' size=10 autofocus> text <input type='text' name='message' size=15 autofocus></p><p><a href=\"Call\"><button>Call</button></a>&nbsp;<a href=\"Send\"><button>Send</button></a></p>";
  /*
  pageLoader.pageRoot  = "<html><body><form action='/' method='POST'><h1>NodeMCU Web Server</h1><br>";
  pageLoader.pageRoot += "Phone&nbsp; &nbsp; &nbsp;<input type='text' name='PHONENUMBER' placeholder='enter here'><br>";
  pageLoader.pageRoot += "Message&nbsp;<input type='text' name='TEXTCONTENT' placeholder='enter here'><br>";
  //pageLoader.pageRoot += "<p><a href=/Call><input name='EXECUTECALL' type='submit' value='Call' /></a>&nbsp; &nbsp; <input name='EXECUTESEND' type='submit' value='Send' /></p></form></body></html>";
  pageLoader.pageRoot += "<p><a href='/Call'><button>Call</button></a>&nbsp;<a href='/Send'><button>Send</button></a></p></form></body></html>";
  pageLoader.pageRoot += "<p>author: thangvpv&nbsp;</p>";

  pageLoader.pageCall  = "<html><body><form action='/Call' method='POST'><h1>NodeMCU Web Server</h1><br>";
  pageLoader.pageCall += "Phone&nbsp; &nbsp; &nbsp;<input type='text' name='PHONENUMBER' placeholder='enter here'><br>";
  pageLoader.pageCall += "Message&nbsp;<input type='text' name='TEXTCONTENT' placeholder='enter here'><br>";
  pageLoader.pageCall += "<p><a href='/Call'><button>Call</button></a>&nbsp;<a href='/Send'><button>Send</button></a></p></form></body></html>";
  pageLoader.pageCall += "<p>author: thangvpv&nbsp;</p>";

  pageLoader.pageSend  = "<html><body><form action='/Send' method='POST'><h1>NodeMCU Web Server</h1><br>";
  pageLoader.pageSend += "Phone&nbsp; &nbsp; &nbsp;<input type='text' name='PHONENUMBER' placeholder='enter here'><br>";
  pageLoader.pageSend += "Message&nbsp;<input type='text' name='TEXTCONTENT' placeholder='enter here'><br>";
  pageLoader.pageSend += "<p><a href='/Call'><button>Call</button></a>&nbsp;<a href='/Send'><button>Send</button></a></p></form></body></html>";
  pageLoader.pageSend += "<p>author: thangvpv&nbsp;</p>";
  */
  pageLoader.pageRoot  = "<html><body><form action='/' method='POST'><h1>NodeMCU Web Server</h1><br>";
  pageLoader.pageRoot += "Phone&nbsp; &nbsp; &nbsp;<input type='text' name='PHONENUMBER' placeholder='enter here'><br>";
  pageLoader.pageRoot += "Message&nbsp;<input type='text' name='TEXTCONTENT' placeholder='enter here'><br>";
  pageLoader.pageRoot += "<p><a href='/Call'><button type='submit' formaction='/Call'>Call</button></a>&nbsp;<a href='/Send'><button type='submit' formaction='/Send'>Send</button></a></p></form></body></html>";
  pageLoader.pageRoot += "<p>author: thangvpv&nbsp;</p>";

}

void codeDebugger() {
  // function trace code
  // display trace
  display.clear();

  display.drawLine(0, 0, 127, 0);
  display.drawLine(0, 2, 127, 2);
  // display title
  display.drawString(40, 3, dispConfig.displayTitle);
  // display ip address
  display.drawString(0, 15, dispConfig.displayIp);
  // display network detail
  display.drawString(0, 32, dispConfig.displayDetail);
  // display notification
  display.drawString(0, 50, dispConfig.displayNotif);
  // display notification status
  display.drawString(108, 50, "[" + dispConfig.dispNotifStatus +"]");

  display.display();
  delay(2000);
}

void sim800aTest() {
    // test SIM800A module function
    digitalWrite(LED2, HIGH);
    delay(5000);
    digitalWrite(LED2, LOW);
    delay(1000);
}

void callFunction() {
    // make call function
    // led signal notif on
    digitalWrite(LED1, LOW);
    if (server.hasArg("PHONENUMBER")) {
      /* code */
      Serial.println(server.arg("PHONENUMBER"));
    }
    delay(3000);
    // led signal notif off
    digitalWrite(LED1, HIGH);
}

void sendFunction() {
    // send messager function
    // led signal notif on
    digitalWrite(LED2, LOW);
    if (server.hasArg("PHONENUMBER") && server.hasArg("TEXTCONTENT")) {
      /* code */
      Serial.println(server.arg("PHONENUMBER"));
      Serial.println(server.arg("TEXTCONTENT"));
    }
    delay(3000);
    // led signal notif off
    digitalWrite(LED2, HIGH);
}
void commandCrafter() {
    // AT command crafter

}

void connectMode() {
    // connect mode when access point mode failed
    dispConfig.displayNotif    = "connecting AP";
    dispConfig.dispNotifStatus = statusPending;
    codeDebugger();
    ESP8266WiFiMulti wifiMulti;
    // first access point information
    wifiMulti.addAP("Dedsec", "29011991");
    wifiMulti.addAP("FPT Software Guest");
    Serial.print("Connecting");
    size_t timeout = 0;
    while (wifiMulti.run() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      display.drawProgressBar(4, 32, 120, 8, timeout);
      display.display();
      timeout++;
    }
    Serial.println("");
    Serial.print("Connected to: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    dispConfig.dispNotifStatus = statusOK;
    dispConfig.displayIp = WiFi.localIP().toString();
    codeDebugger();
}
