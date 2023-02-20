/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* host = "maquina-lavar";
const char* ssid = "@wifi";
const char* password = "Gabi&Bruno_02!04";
const int delta_time = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


WebServer server(80);
const char* serverIndex = 
 "<form method='POST' action='/update' enctype='multipart/form-data'>"
 "<input type='file' name='update'><input type='submit' value='Update'></form><a href='/teste'>teste</a>";


void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  timeClient.begin();
  timeClient.setTimeOffset(-3*3600);
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    MDNS.begin(host);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/teste", HTTP_GET, []() {
      server.send(200, "text/html", "<p>TESTE</p>");
    });
    
    server.on("/update", HTTP_POST, []() {
      Serial.println("FUNCIONA!");
      
      while(true){
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);
      }  
      //server.sendHeader("Connection", "close");
      //server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      //ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin()) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      } else {
        Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
      }
    });
    server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Failed");
  }
}

void loop(void) {
  server.handleClient();
  delay(1);
}

void washactive(washTime){
  formattedDate = timeClient.getFormattedDate();
  washTime = ;
  sleep(formattedDate - washTime);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}
