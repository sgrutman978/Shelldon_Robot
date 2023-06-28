#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <string.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "index.h"

#ifndef STASSID
String stassid = "Connecto_Patronum"; //"sgrutman978"; 
String stapsk = "Imustnottelllies1997"; //"nimbus1234"
#endif

ESP8266WebServer server(80);

// Motor A connections (RIGHT)
int enA = 10;
int in1 = 15;
int in2 = 14;
// Motor B connections (LEFT)
int in3 = 13;
int in4 = 12;
int enB = 16;

int speed1 = 210;
int speed2 = 210;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

void setup() {
Serial.begin(9600);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  initMotorPins();
  serverSetup();
  printWifiOnLCD();
}

void loop() {
  server.handleClient(); 
}

void initMotorPins() {
  // Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);

  //Set init speed
  analogWrite(enA, speed2); //RIGHT
	analogWrite(enB, speed1); //LEFT
}

void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);
}

void handleRequest(uint8_t input1, uint8_t input2, uint8_t input3, uint8_t input4, String message){
    digitalWrite(in1, input1);
	  digitalWrite(in2, input2);
	  digitalWrite(in3, input3);
	  digitalWrite(in4, input4);
    server.send(200, "text/html", message);
}

// void handleForward(){
//               digitalWrite(in1, HIGH);
// 	      digitalWrite(in2, LOW);
// 	      digitalWrite(in3, HIGH);
// 	      digitalWrite(in4, LOW);
//         server.send(200, "text/html", "fuck this");
// }

void serverSetup(){
  WiFi.begin(stassid, stapsk);     //Connect to your WiFi router

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.print("Connected to ");
  Serial.println(stassid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Receive an HTTP GET request
  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/forward", [](){handleRequest(HIGH, LOW, HIGH, LOW, "Forward");});
  server.on("/backward", [](){handleRequest(LOW, HIGH, LOW, HIGH, "Backward");});
  server.on("/stop", [](){handleRequest(LOW, LOW, LOW, LOW, "Stop");});
  server.on("/left", [](){handleRequest(LOW, HIGH, LOW, LOW, "Left");});
  server.on("/right", [](){handleRequest(LOW, LOW, LOW, HIGH, "Right");});
  server.on("/upl", [](){changeSpeed(1, "Up Left");});
  server.on("/downl", [](){changeSpeed(2, "Down Left");});
  server.on("/upr", [](){changeSpeed(3, "Up Right");});
  server.on("/downr", [](){changeSpeed(4, "Down Right");});

  // server.onNotFound(notFound);

  server.begin(); //Start server
  Serial.println("HTTP server started");
}


// void notFound(AsyncWebServerRequest *request) {
//   request->send(404, "text/plain", "Not found");
// }


void printWifiOnLCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());
}


void changeSpeed(uint8_t mode, String message){
  uint8_t speedChange = 5;
      if mode == 1: { //UL
        speed1 = speed1 + speedChange;
      }
      if mode == 2: { //DL
        speed1 = speed1 - speedChange;
      }
      if mode == 3: { //UR
        speed2 = speed2 + speedChange;
      }
      if mode == 4: { //DR
        speed2 = speed2 - speedChange;
      }
      server.send(200, "text/html", message);
}
