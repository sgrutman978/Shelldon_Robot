#include <stdio.h>
#include <string.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

#include <Wire.h>

// Define analog input
#define SIGNAL_PIN A0
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0;
// Float for Reference Voltage
float ref_voltage = 5.0;
// Integer for ADC value
int adc_value = 0;

const char* ssid = "Connecto_Patronum";//"sgrutman978"; //Enter SSID
const char* password = "Imustnottelllies1997";//"nimbus1234"; //Enter Password
const char* websockets_server_host = "stevengrutman.com"; //Enter server adress
const uint16_t websockets_server_port = 56112; // Enter server port

using namespace websockets;
WebsocketsClient client;

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

int counter = 0;


void setup() {
  Serial.begin(9600);
  initMotorPins();
  wifiSetup();
  wsClientSetup();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiSetup();
  }
  if(!client.available()){
    wsClientSetup();
  }
  // let the websockets client check for incoming messages
  if(client.available()) {
    client.poll();
  }

counter += 1;
if(counter > 50){
    // Read the Analog Input
  adc_value = analogRead(SIGNAL_PIN);
  // Determine voltage at ADC input
  adc_voltage  = (adc_value * ref_voltage) / 1024.0;
  // Calculate voltage at divider input
  in_voltage = adc_voltage / (R2 / (R1 + R2)) ;
  // Print results to Serial Monitor to 2 decimal places
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);
  client.send(String(in_voltage));
  counter = 0;
}

  delay(100);
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
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  //Set init speed
  analogWrite(enA, speed2); //RIGHT
	analogWrite(enB, speed1); //LEFT
}


void wifiSetup(){
  WiFi.begin(ssid, password); //Connect to your WiFi router

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  Serial.println("Connected to Wifi");
}


void wsClientSetup(){
  Serial.println("\n\nConnecting to server");
    // try to connect to Websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    if(connected) {
        Serial.println("Connecetd!");
        client.send("Hello Server");
    } else {
        Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
        handleCommand(message.data());
    });  
}


void handleCommand(String command){
      if (command == "forward") {
        handleMotor(HIGH, LOW, HIGH, LOW);
      }
      if (command == "backward") {
        handleMotor(LOW, HIGH, LOW, HIGH);
      }
      if (command == "stop") {
        handleMotor(LOW, LOW, LOW, LOW);
      }
      if (command == "left") {
        handleMotor(LOW, HIGH, LOW, LOW);
      }
      if (command == "right") {
        handleMotor(LOW, LOW, LOW, HIGH);
      }
      if (command == "upl") {
        handleSpeed(1);
      }
      if (command == "downl") {
        handleSpeed(2);
      }
      if (command == "upr") {
        handleSpeed(3);
      }
      if (command == "downr") {
        handleSpeed(4);
      }
}


void handleMotor(uint8_t input1, uint8_t input2, uint8_t input3, uint8_t input4){
    digitalWrite(in1, input1);
	  digitalWrite(in2, input2);
	  digitalWrite(in3, input3);
	  digitalWrite(in4, input4);
}


void handleSpeed(uint8_t mode){
  uint8_t speedChange = 5;
      if (mode == 1) { //UL
        speed1 = speed1 + speedChange;
      }
      if (mode == 2) { //DL
        speed1 = speed1 - speedChange;
      }
      if (mode == 3) { //UR
        speed2 = speed2 + speedChange;
      }
      if (mode == 4) { //DR
        speed2 = speed2 - speedChange;
      }
      analogWrite(enA, speed2); //RIGHT
	    analogWrite(enB, speed1); //LEFT
}
