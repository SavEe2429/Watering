// Include Library
#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <LittleFS.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>

// Setup pin

#define SS_PIN D8
#define RST_PIN D0
#define DIO0_PIN D2

// Declare var
const char *ssid = "Dounlop ";
const char *pass = "94502244";
const char *serverName = "https://watering-9xrq.onrender.com";
int httpResponseCode;
int httpResponseCodeDHT;
String prev_status = " ";

bool deviceStatus = false;
unsigned long lastTime = 0;
const unsigned long interval = 1500;
bool resendCommand = false;
bool sendCommand = false;
bool receiveAck = false;
String status;
String temp;
String hum;

// Use Server
//  AsyncWebServer server(80);
HTTPClient http;
WiFiClientSecure client;

void ConnectWiFi()
{
  WiFi.begin(ssid, pass);

  // Try to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  // Connected!
  Serial.println(" ");
  Serial.println("Connected! LocalIP : ");
  Serial.print(WiFi.localIP());
}

void EnableLora()
{
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  Serial.print("Starting Lora.");
  while (!LoRa.begin(433E6))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("Starting Lora Success!");
}

// void EnableLittleFS(){
//     //Enable LittleFS
//     if(!LittleFS.begin()){
//       Serial.println("LittleFS Mount Failed!");
//       return;
//     }
// }

// void recentCommand(int Left, int Right)
// {
//   if (Left == 1 && Right == 0)
//   {
//     digitalWrite(Relay_R, LOW);
//     digitalWrite(Relay_L, HIGH);
//   }
//   else if (Left == 0 && Right == 1)
//   {
//     digitalWrite(Relay_L, LOW);
//     digitalWrite(Relay_R, HIGH);
//   }
//   else if (Left == 0 && Right == 0)
//   {
//     digitalWrite(Relay_L, LOW);
//     digitalWrite(Relay_R, LOW);
//   }

//   deviceStatus = true;
// }

void changePath(String paht, String value1, String value2)
{
  http.begin(client, String(serverName) + paht);
  http.addHeader("Content-Type", "application/json");
  if (paht == "/command")
  {
    httpResponseCode = http.POST("{\"action\": \"" + value1 + "\" ,\"from\" : \"ESP\"}");
  }
  else if (paht == "/dhtvalue")
  {
    httpResponseCodeDHT = http.POST("{\"humidity\": \"" + value1 + "\" ,\"temperature\": \"" + value2 + "\" ,\"from\" : \"ESP\"}");
  }
}

bool Timeout(bool receiveAck)
{
  if (!receiveAck && millis() - lastTime >= interval)
  {
    lastTime = millis();
    return true;
  }
  else if (receiveAck)
  {
    lastTime = millis();
  }

  return false;
}

// Void Setup
void setup()
{
  // Setup
  Serial.begin(115200);
  EnableLora();
  ConnectWiFi();

  // LoRa.setTxPower(20);
  // EnableLittleFS();

  // Setup Initial
  // pinMode(Lim_Switch_L, INPUT_PULLUP); // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  // pinMode(Lim_Switch_R, INPUT_PULLUP); // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  // pinMode(Relay_L, OUTPUT);
  // pinMode(Relay_R, OUTPUT);
  // digitalWrite(Relay_R, LOW);
  // digitalWrite(Relay_L, LOW);

  // server.serveStatic("/" , LittleFS , "/").setDefaultFile("index.html");

  // server.on("/on" , HTTP_GET , [](AsyncWebServerRequest *request){
  //   String jsonResponse = "{\"status\":\"ON\"}";
  //   request->send(200 , "application/json" , jsonResponse);
  //   recentCommand(1,0);
  // });

  // server.on("/off", HTTP_GET , [](AsyncWebServerRequest *request){
  //   recentCommand(0,1);
  // });

  // server.on("/status" , HTTP_GET , [](AsyncWebServerRequest *request){
  //   String jsonResponse = "{\"status\" : \"" + String(deviceStatus ? "เปิด" : "ปิด") + "\"}";
  //   Serial.println(jsonResponse); // Debug
  //   request->send(200 , "application/json" , jsonResponse);
  // });
  // server.onNotFound([](AsyncWebServerRequest *request){
  //   request->redirect("https://savee2429.github.io/Watering/");
  // });

  // server.begin();
}

// Void Loop
void loop()
{
  client.setInsecure();
  if (WiFi.status() == WL_CONNECTED)
  {

    http.begin(client, String(serverName) + "/recent");
    http.addHeader("Content-Type", "application/json");

    httpResponseCode = http.GET();
    Serial.print("HTTP_Response : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode >= 0)
    {
      String response = http.getString();
      Serial.println("Server Response : " + response);

      StaticJsonDocument<512> doc;
      deserializeJson(doc, response);
      status = doc["command"].as<String>();

      if (prev_status != status)
      {
        printf("Prev_Status: %s , Status: %s\n", prev_status.c_str(), status.c_str());
        sendCommand = false;
        receiveAck = false;
        prev_status = status;
      }
      if (status == "ON")
      {
        if (!sendCommand || resendCommand)
        {
          LoRa.beginPacket();
          LoRa.print(status);
          LoRa.endPacket();
          Serial.println("Send ON");
          sendCommand = true;
          resendCommand = false;
        }
        else if (Timeout(receiveAck))
        {
          resendCommand = true;
        }
      }
      else if (status == "OFF")
      {
        if (!sendCommand || resendCommand)
        {
          LoRa.beginPacket();
          LoRa.print(status);
          LoRa.endPacket();
          Serial.println("Send OFF");
          sendCommand = true;
          resendCommand = false;
        }
        else if (Timeout(receiveAck))
        {
          resendCommand = true;
        }
      }
      else if (status == "STOP")
      {

        if (!sendCommand || resendCommand)
        {
          LoRa.beginPacket();
          LoRa.print(status);
          LoRa.endPacket();
          Serial.println("Send STOP");
          sendCommand = true;
          resendCommand = false;
        }
        else if (Timeout(receiveAck))
        {
          resendCommand = true;
        }
      }

      // if (digitalRead(Lim_Switch_L) == LOW && digitalRead(Relay_L) == HIGH)
      // {
      //   if (!hasSendPost)
      //   {
      //     digitalWrite(Relay_L, LOW);
      //     changePath("STOP");
      //     hasSendPost = true;
      //   }
      // }
      // else
      // {
      //   hasSendPost = false;
      // }

      // if (digitalRead(Lim_Switch_R) == LOW && digitalRead(Relay_R) == HIGH)
      // {
      //   if (!hasSendPost)
      //   {
      //     digitalWrite(Relay_R, LOW);
      //     changePath("STOP");
      //     hasSendPost = true;
      //   }
      // }
      // else
      // {
      //   hasSendPost = false;
      // }
    }
    else
    {
      Serial.println("HTTP ERROR " + String(httpResponseCode) + ": " + http.errorToString(httpResponseCode));
    }
    http.end();

    int packetSize = LoRa.parsePacket();
    Serial.print("PacketSize : ");
    Serial.print(packetSize);
    Serial.print(", with RSSI ");
    Serial.println(LoRa.packetRssi());

    if (packetSize > 0)
    {

      while (LoRa.available())
      {
        String rawdata = LoRa.readString();
        

        if (rawdata.startsWith("State"))
        {
          String data = rawdata.substring(5);
          Serial.println("rawdata " + rawdata + " , data: " + data);
          if (data == status || data == "STOP" )
          {
            changePath("/command", data, "");
            receiveAck = true;
          }
        }
        // else if(rawdata.startsWith("DHT")){
        //   String data = rawdata.substring(3);
        //   int commaIndex = data.indexOf(',');
        //   temp = data.substring(0,commaIndex);
        //   hum = data.substring(commaIndex + 1);
        //   changePath("/dhtvalue" , hum ,temp);
        //   Serial.println("rawdata " + rawdata + " , data: " + data);
        // }
        
      }
    }
  }

  printf("Sendcommand: %s , Resendcommand: %s , ReceiveAck: %s , Prev_Status: %s , Status: %s\n",
         sendCommand ? "true" : "false",
         resendCommand ? "true" : "false",
         receiveAck ? "true" : "false",
         prev_status.c_str(), status.c_str());

  // Serial.print("Lim_Switch_R : ");
  // Serial.println(digitalRead(Lim_Switch_R));
  // Serial.print("Lim_Switch_L : ");
  // Serial.println(digitalRead(Lim_Switch_L));
  // Serial.print("Relay_R : ");
  // Serial.println(digitalRead(Relay_R));
  // Serial.print("Relay_L : ");
  // Serial.println(digitalRead(Relay_L));
  delay(200);
}
