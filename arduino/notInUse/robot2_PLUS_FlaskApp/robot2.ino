/*
  UDPSendReceive.pde:
  This sketch receives UDP message strings, prints them to the serial port
  and sends an "acknowledge" string back to the sender

  A Processing sketch is included at the end of file that can be used to send
  and received messages for testing with a computer.

  created 21 Aug 2010
  by Michael Margolis

  This code is in the public domain.

  adapted from Ethernet library examples
*/


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <string.h>

#ifndef STASSID
String stassid = "sgrutman978"; //"Connecto_Patronum"
String stapsk = "nimbus1234"; //"Imustnottelllies1997"
#endif

//m = motor, w = wifi
#define FORWARD 102 //f
#define BACKWARD 98 //b
#define LEFT 108 //l
#define RIGHT 114 //r
#define STOP 115 //s
#define UP_SPEED_L 49 //1
#define DOWN_SPEED_L 50 //2
#define UP_SPEED_R 51 //3
#define DOWN_SPEED_R 52 //4
#define UP_SPEED_A 117 //u
#define DOWN_SPEED_A 100 //d

unsigned int localPort = 8888;  // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\r\n";        // a string to send back

WiFiUDP Udp;

// Motor A connections (RIGHT)
int enA = 10;
int in1 = 1;
int in2 = 14;
// Motor B connections (LEFT)
int in3 = 13;
int in4 = 12;
int enB = 16;

int speed1 = 160;
int speed2 = 160;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

void setup() {

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  initMotorPins();
  wifiSetup();
}

void loop() {
  // lcdPrint();
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort(), Udp.destinationIP().toString().c_str(), Udp.localPort(), ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    // Serial.printf("Length %d", n);
    packetBuffer[n] = 0;
    // Serial.println("Contents:");
    int command_type = packetBuffer[0];
    Serial.print(command_type);
    if(command_type == 109){ //motor
      processMotorCommand(packetBuffer[1]);
    }
    if(command_type == 119){ //wifi
      processWifiCommand(&packetBuffer[1]);
    }
    // Serial.println(&packetBuffer[1]); //char*
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}

void wifiSetup(){
    // set cursor to first column, first row
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(stassid, stapsk);
  Serial.print('\n');
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  lcd.print(WiFi.localIP());
  Serial.print("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
}

void lcdPrint() {
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, World!");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Hello, World!");
  delay(1000);
  lcd.clear(); 
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
}

void processWifiCommand(char* command){
  Serial.print(command);
  //reference for below -> https://cplusplus.com/reference/cstring/strtok/
  char * split = strtok(command, "~");
  stassid = split;
  Serial.print(split);
  split = strtok(NULL, "~");
  stapsk = split;
  Serial.print("aaaahahahahahah");
  Serial.print(split);
  wifiSetup();
}

void processMotorCommand(int command) {
  Serial.print(command);
  switch (command) {
      case FORWARD: {
        //FORWARD
        Serial.print("mehhhh");
        digitalWrite(in1, HIGH);
	      digitalWrite(in2, LOW);
	      digitalWrite(in3, HIGH);
	      digitalWrite(in4, LOW);
        break;
      }
      case BACKWARD: {
        //BACKWARDS
        digitalWrite(in1, LOW);
	      digitalWrite(in2, HIGH);
	      digitalWrite(in3, LOW);
	      digitalWrite(in4, HIGH);
        break;
      }
      case LEFT: {
        //LEFT
        digitalWrite(in1, LOW);
	      digitalWrite(in2, HIGH);
	      digitalWrite(in3, LOW);
	      digitalWrite(in4, LOW);
        break;
      }
      case RIGHT: {
        //RIGHT
        digitalWrite(in1, LOW);
	      digitalWrite(in2, LOW);
	      digitalWrite(in3, LOW);
	      digitalWrite(in4, HIGH);
        break;
      }
      case STOP: {
        //STOP
        digitalWrite(in1, LOW);
	      digitalWrite(in2, LOW);
	      digitalWrite(in3, LOW);
	      digitalWrite(in4, LOW);
        break;
      }
      case UP_SPEED_L: {
        speed1 = speed1 + 5;
        break;
      }
      case DOWN_SPEED_L: {
        speed1 = speed1 - 5;
        break;
      }
      case UP_SPEED_R: {
        speed2 = speed2 + 5;
        break;
      }
      case DOWN_SPEED_R: {
        speed2 = speed2 - 5;
        break;
      }
      case UP_SPEED_A: {
        speed1 = speed1 + 5;
        speed2 = speed2 + 5;
        break;
      }
      case DOWN_SPEED_A: {
        speed1 = speed1 - 5;
        speed2 = speed2 - 5;
        break;
      }
      default: {
        Serial.println("Button not recognized");
      }
    }
    if(speed1 > 255){
      speed1 = 255;
    }
    if(speed1 < 0){
      speed1 = 0;
    }
    if(speed2 > 255){
      speed2 = 255;
    }
    if(speed2 < 0){
      speed2 = 0;
    }
    analogWrite(enA, speed2); //RIGHT
	  analogWrite(enB, speed1); //LEFT
}


/*
  test (shell/netcat):
  --------------------
    nc -u 192.168.esp.address 8888
*/
