// Include Library
#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <LittleFS.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Setup pin
#define Relay_L D3
#define Relay_R D2
#define Lim_Switch_L D6
#define Lim_Switch_R D7

// Declare var
const char *ssid = "Dounlop ";
const char *pass = "94502244";
const char *serverName = "https://watering-9xrq.onrender.com";

bool deviceStatus = false;

// Use Server
//  AsyncWebServer server(80);

void ConnectWiFi()
{
  WiFi.begin(ssid, pass);

  // Try to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Connected!
  Serial.println(" ");
  Serial.println("Connected! LocalIP : ");
  Serial.print(WiFi.localIP());
}

// void EnableLittleFS(){
//     //Enable LittleFS
//     if(!LittleFS.begin()){
//       Serial.println("LittleFS Mount Failed!");
//       return;
//     }
// }

void recentCommand(int Left, int Right)
{
  if (Left == 1 && Right == 0)
  {
    digitalWrite(Relay_R, LOW);
    digitalWrite(Relay_L, HIGH);
  }
  else if (Left == 0 && Right == 1)
  {
    digitalWrite(Relay_L, LOW);
    digitalWrite(Relay_R, HIGH);
  }

  deviceStatus = true;
}

// Void Setup
void setup()
{
  // Setup
  Serial.begin(115200);

  ConnectWiFi();
  // EnableLittleFS();

  // Setup Initial
  pinMode(Lim_Switch_L, INPUT_PULLUP); // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  pinMode(Lim_Switch_R, INPUT_PULLUP); // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  pinMode(Relay_L, OUTPUT);
  pinMode(Relay_R, OUTPUT);
  digitalWrite(Relay_R, LOW);
  digitalWrite(Relay_L, LOW);

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

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;
    
    http.begin(client ,String(serverName) + "/command");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("{}");

    if (httpResponseCode >= 0)
    {
      String response = http.getString();
      Serial.println("Server Response : " + response);

      if (response.indexOf("ON") >= 0)
      {
        recentCommand(1,0);
      }
      else if (response.indexOf("OFF") >= 0)
      {
        recentCommand(0,1);
      }

      if (digitalRead(Lim_Switch_L) == LOW && digitalRead(Relay_L) == HIGH)
      {
        Serial.println("IN Left");

        digitalWrite(Relay_L, LOW);
      }

      if (digitalRead(Lim_Switch_R) == LOW && digitalRead(Relay_R) == HIGH)
      {
        Serial.println("IN Right");

        digitalWrite(Relay_R, LOW);
      }
    }
    else
    {
      Serial.printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }

  Serial.print("Lim_Switch_R : ");
  Serial.println(digitalRead(Lim_Switch_R));
  Serial.print("Lim_Switch_L : ");
  Serial.println(digitalRead(Lim_Switch_L));
  Serial.print("Relay_R : ");
  Serial.println(digitalRead(Relay_R));
  Serial.print("Relay_L : ");
  Serial.println(digitalRead(Relay_L));

  delay(1000);
}
