#include <stdio.h>
#include <string.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include <Wire.h>
#include <ESP8266WebServer.h>

// // Define analog input
// #define SIGNAL_PIN A0
// // Floats for ADC voltage & Input voltage
// float adc_voltage = 0.0;
// float in_voltage = 0.0;
// // Floats for resistor values in divider (in ohms)
// float R1 = 30000.0;
// float R2 = 7500.0;
// // Float for Reference Voltage
// float ref_voltage = 5.0;
// // Integer for ADC value
// int adc_value = 0;

const char* ssid = "sgrutman978";//"HYUNDAI-GUEST"; //"sgrutman978";//"Connecto_Patronum"; //Enter SSID
const char* password = "nimbus1234";//"welcome2020a"; //"Imustnottelllies1997";//"nimbus1234"; //Enter Password
const char* websockets_server_host = "stevengrutman.com"; //Enter server adress
const uint16_t websockets_server_port = 56112; // Enter server port

using namespace websockets;
WebsocketsClient client;

// // Motor A connections (RIGHT)
// int enA = 1;
// int in1 = 12;
// int in2 = 11;
// // Motor B connections (LEFT)
// int in3 = 10;
// int in4 = 9;
// int enB = 0;

// // Motor A connections (RIGHT)
// int enA = 10;
// int in1 = 15;
// int in2 = 14;
// // Motor B connections (LEFT)
// int in3 = 13;
// int in4 = 12;
// int enB = 0;

// Motor A connections (RIGHT)
int enA = 10;
int in1 = 15;
int in2 = 2;
// Motor B connections (LEFT)
int in3 = 3;
int in4 = 1;
int enB = 0;

int speed1 = 210;
int speed2 = 210;

int counter = 0;

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <WiFiUdp.h>
unsigned int localPort = 8888;  // local port to listen on
// buffers for receiving and sending data
WiFiUDP Udp;
char *udpRemoteIP = "104.236.68.131";
uint16_t udpRemotePort = 3004;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV2640_MINI_2MP)||defined (OV5640_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_PLUS) \
    || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
    ||(defined (ARDUCAM_SHIELD_V2) && (defined (OV2640_CAM) || defined (OV5640_CAM) || defined (OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif
// set GPIO16 as the slave select :
const int CS = 16;

//you can change the value of wifiType to select Station or AP mode.
//Default is AP mode.
int wifiType = 0; // 0:Station  1:AP

//AP mode configuration
//Default is arducam_esp8266.If you want,you can change the AP_aaid  to your favorite name
const char *AP_ssid = "arducam_esp8266";
//Default is no password.If you want to set password,put your password here
const char *AP_password = "";

static const size_t bufferSize = 4096;
static uint8_t buffer[bufferSize] = {0xFF};
static uint8_t buffer2[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
int j = 0;
bool is_header = false;

ESP8266WebServer server(80);
#if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
ArduCAM myCAM(OV2640, CS);
#elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
ArduCAM myCAM(OV5640, CS);
#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
ArduCAM myCAM(OV5642, CS);
#endif


void start_capture() {
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM) {
  WiFiClient client = server.client();
  uint32_t len  = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  if (!client.connected()) return;
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-len: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  i = 0;
  j = 0;
  while ( len-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
buffer2[j++] = temp;
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buffer[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      if (!client.connected()) break;
      client.write(&buffer[0], i);

      Udp.beginPacket(udpRemoteIP, udpRemotePort);
    Udp.write(&buffer2[0], j);
    Udp.endPacket();

      is_header = false;
      i = 0;
      j = 0;
      myCAM.CS_HIGH();
      break;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < bufferSize)
        buffer[i++] = temp;
      else
      {
        //Write bufferSize bytes image data to file
        if (!client.connected()) break;
        client.write(&buffer[0], bufferSize);

      Udp.beginPacket(udpRemoteIP, udpRemotePort);
    Udp.write(&buffer[0], bufferSize);
    Udp.endPacket();

        i = 0;
        buffer[i++] = temp;
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;
    }
  }
}

void serverCapture() {
  char meh[] = "mehhh";
        Udp.beginPacket(udpRemoteIP, udpRemotePort);
    Udp.write(meh);
    Udp.endPacket();
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  start_capture();
  Serial.println(F("CAM Capturing"));
  int total_time = 0;
  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  total_time = 0;
  Serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print(F("send total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  Serial.println(F("CAM send Done."));
}

void serverStream() {
  WiFiClient client = server.client();

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (1) {
    start_capture();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
    size_t len = myCAM.read_fifo_length();
    if (len >= MAX_FIFO_SIZE) //8M
    {
      Serial.println(F("Over size."));
      continue;
    }
    if (len == 0 ) //0 kb
    {
      Serial.println(F("Size is 0."));
      continue;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    if (!client.connected()) {
      Serial.println("break"); break;
    }
    response = "--frame\r\n";
    response += "Content-Type: image/jpeg\r\n\r\n";
    server.sendContent(response);
    while ( len-- )
    {
      temp_last = temp;
      temp =  SPI.transfer(0x00);

      //Read JPEG data from FIFO
      if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
      {
        buffer[i++] = temp;  //save the last  0XD9
        //Write the remain bytes in the buffer
        myCAM.CS_HIGH();;
        if (!client.connected()) {
          client.stop(); is_header = false; break;
        }
        client.write(&buffer[0], i);

      Udp.beginPacket(udpRemoteIP, udpRemotePort);
    Udp.write(&buffer[0], i);
    Udp.endPacket();

        is_header = false;
        i = 0;
      }
      if (is_header == true)
      {
        //Write image data to buffer if not full
        if (i < bufferSize)
          buffer[i++] = temp;
        else
        {
          //Write bufferSize bytes image data to file
          myCAM.CS_HIGH();
          if (!client.connected()) {
            client.stop(); is_header = false; break;
          }
          client.write(&buffer[0], bufferSize);

      Udp.beginPacket(udpRemoteIP, udpRemotePort);
    Udp.write(&buffer[0], bufferSize);
    Udp.endPacket();

          i = 0;
          buffer[i++] = temp;
          myCAM.CS_LOW();
          myCAM.set_fifo_burst();
        }
      }
      else if ((temp == 0xD8) & (temp_last == 0xFF))
      {
        is_header = true;
        buffer[i++] = temp_last;
        buffer[i++] = temp;
      }
    }
    if (!client.connected()) {
      client.stop(); is_header = false; break;
    }
  }
}

void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);

  if (server.hasArg("ql")) {
    int ql = server.arg("ql").toInt();
#if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
    myCAM.OV2640_set_JPEG_size(ql);
#elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
    myCAM.OV5640_set_JPEG_size(ql);
#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
    myCAM.OV5642_set_JPEG_size(ql);
#endif
    delay(1000);
    Serial.println("QL change to: " + server.arg("ql"));
  }
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);//9600);
  initMotorPins();
  // wifiSetup(); //wifi now gets setup in cameraSetup() below based on wifi type
  //     need to remove obsolete stuff though once settled
  // wsClientSetup();
  cameraSetup();
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

// Voltage Collection
// counter += 1;
// if(counter > 50){
//     // Read the Analog Input
//   adc_value = analogRead(SIGNAL_PIN);
//   // Determine voltage at ADC input
//   adc_voltage  = (adc_value * ref_voltage) / 1024.0;
//   // Calculate voltage at divider input
//   in_voltage = adc_voltage / (R2 / (R1 + R2)) ;
//   // Print results to Serial Monitor to 2 decimal places
//   Serial.print("Input Voltage = ");
//   Serial.println(in_voltage, 2);
//   client.send(String(in_voltage));
//   counter = 0;
// }
  server.handleClient();
  // delay(100);
}


void cameraSetup(){
    uint8_t vid, pid;
  uint8_t temp;
#if defined(_SAM3X8E_)
  Wire1.begin();
#else
  Wire.begin();
#endif
  // Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));
  // set the CS as an output:
  pinMode(CS, OUTPUT);
  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000); //4MHz
  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println(F("SPI1 interface Error!"));
    while (1);
  }
#if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
  //Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    Serial.println(F("Can't find OV2640 module!"));
  else
    Serial.println(F("OV2640 detected."));
#elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
  //Check if the camera module type is OV5640
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x40))
    Serial.println(F("Can't find OV5640 module!"));
  else
    Serial.println(F("OV5640 detected."));
#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
  //Check if the camera module type is OV5642
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x42)) {
    Serial.println(F("Can't find OV5642 module!"));
  }
  else
    Serial.println(F("OV5642 detected."));
#endif


  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
#if defined (OV2640_MINI_2MP) || defined (OV2640_CAM)
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
#elif defined (OV5640_MINI_5MP_PLUS) || defined (OV5640_CAM)
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5640_320x240);
#elif defined (OV5642_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) ||(defined (OV5642_CAM))
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5642_320x240);
#endif

  myCAM.clear_fifo_flag();
  if (wifiType == 0) {
    if (!strcmp(ssid, "SSID")) {
      Serial.println(F("Please set your SSID"));
      while (1);
    }
    if (!strcmp(password, "PASSWORD")) {
      Serial.println(F("Please set your PASSWORD"));
      while (1);
    }
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());

Udp.begin(localPort);

  } else if (wifiType == 1) {
    Serial.println();
    Serial.println();
    Serial.print(F("Share AP: "));
    Serial.println(AP_ssid);
    Serial.print(F("The password is: "));
    Serial.println(AP_password);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid, AP_password);
    Serial.println("");
    Serial.println(WiFi.softAPIP());
  }

  // Start the server
  server.on("/capture", HTTP_GET, serverCapture);
  server.on("/stream", HTTP_GET, serverStream);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println(F("Server started"));
}




void initMotorPins() {
  // Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	// pinMode(in4, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	// digitalWrite(in4, LOW);

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
