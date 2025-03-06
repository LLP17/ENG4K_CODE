#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char *ssid_AP = "ESP32-AP";
const char *password_AP = "12345678";

IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 10);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    delay(2000);

    // Initialize SPIFFS for file storage
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Configure and start the access point
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    if (WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("AP Config Ready");
    } else {
        Serial.println("AP Config Failed!");
    }

    if (WiFi.softAP(ssid_AP, password_AP)) {
        Serial.println("AP Started");
        Serial.println("IP Address: " + WiFi.softAPIP().toString());
    } else {
        Serial.println("AP Failed to Start!");
    }

    // Serve static files
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

    // Start the server
    server.begin();
    Serial.println("Server started!");
}

void loop() {
    // No code needed here for this example
}
