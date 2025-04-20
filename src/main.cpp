//Include Library
#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Setup pin
#define Relay_L D3
#define Relay_R D2
#define Lim_Switch_L D6
#define Lim_Switch_R D7

//Declare var
const char *ssid = "Dounlop ";
const char *pass = "94502244";
bool deviceStatus = false;

//Use Server
AsyncWebServer server(80);

void ConnectWiFi(){
  WiFi.begin(ssid ,pass);

  //Try to connect
  while(WiFi.status() != WL_CONNECTED){
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

//Void Setup
void setup()
{
  // Setup
  Serial.begin(115200);

  ConnectWiFi();
  // EnableLittleFS();


  //Setup Initial
  pinMode(Lim_Switch_L, INPUT_PULLUP);  // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  pinMode(Lim_Switch_R, INPUT_PULLUP);  // ถ้าใช้ปุ่มทั่วไป แนะนำ INPUT_PULLUP
  pinMode(Relay_L, OUTPUT);
  pinMode(Relay_R, OUTPUT);
  digitalWrite(Relay_R,LOW);
  digitalWrite(Relay_L,LOW);


  // server.serveStatic("/" , LittleFS , "/").setDefaultFile("index.html");


  server.on("/on" , HTTP_GET , [](AsyncWebServerRequest *request){
    deviceStatus = true;
    String jsonResponse = "{\"status\":\"ON\"}";
    request->send(200 , "application/json" , jsonResponse);
  });

  server.on("/off", HTTP_GET , [](AsyncWebServerRequest *request){
    deviceStatus = false;
    String jsonResponse = "{\"status\":\"OFF\"}";
    request->send(200 , "application/json" , jsonResponse);
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("https://google.com");
  });

  server.begin();
}

//Void Loop
void loop()
{
  digitalWrite(Relay_R,HIGH);

  if(digitalRead(Lim_Switch_L) == LOW && digitalRead(Relay_L) == HIGH){
    Serial.println("IN Left");
    digitalWrite(Relay_L, LOW);
  }
  if(digitalRead(Lim_Switch_R) == LOW && digitalRead(Relay_R) == HIGH){
    Serial.println("IN Right");
    digitalWrite(Relay_R, LOW);
  }
  delay(1000);
}
