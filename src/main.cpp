// Include Library
#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <LittleFS.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Setup pin
#define Relay_L D3
#define Relay_R D2
#define Lim_Switch_L D6
#define Lim_Switch_R D7

// Declare var
const char *ssid = "Dounlop ";
const char *pass = "94502244";
const char *serverName = "https://watering-9xrq.onrender.com";
static bool hasSendPost = false;
bool deviceStatus = false;

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

void changePath(String state)
{
  http.begin(client, String(serverName) + "/command");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"action\": \"" + state + "\" ,\"from\" : \"esp\"}");

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error in HTTP request: ");
    Serial.println(httpResponseCode);
  }
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
  client.setInsecure();
  if (WiFi.status() == WL_CONNECTED)
  {
    http.begin(client, String(serverName) + "/recent");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.GET();
    Serial.print("HTTP_Response : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode >= 0)
    {
      String response = http.getString();

      Serial.println("Server Response : " + response);

      if (response.indexOf("ON") >= 0)
      {
        recentCommand(1, 0);
        changePath("ON");
      }
      else if (response.indexOf("OFF") >= 0)
      {
        recentCommand(0, 1);
        changePath("OFF");
      }

      if (digitalRead(Lim_Switch_L) == LOW && digitalRead(Relay_L) == HIGH)
      {
        if (!hasSendPost)
        {
          digitalWrite(Relay_L, LOW);
          changePath("STOP");

          hasSendPost = true;
        }
      }
      else
      {
        hasSendPost = false;
      } 

      if (digitalRead(Lim_Switch_R) == LOW && digitalRead(Relay_R) == HIGH)
      {
        if (!hasSendPost)
        {
          digitalWrite(Relay_R, LOW);
          changePath("STOP");

          hasSendPost = true;
        }
      }
      else
      {
        hasSendPost = false;
      }
    }
    else
    {
      Serial.println("HTTP ERROR " + String(httpResponseCode) + ": " + http.errorToString(httpResponseCode));
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

  delay(200);
}
