#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

const char *ssid_AP = "ECHO-AP";
const char *password_AP = "ourbread";
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/* Motor Configurations */

// Motor Pins
int motor1Pin1 = 21; 
int motor1Pin2 = 22; 
int enable1Pin = 23;
int motor2Pin1 = 5; 
int motor2Pin2 = 19; 
int enable2Pin = 18;

const int freq = 30000;    // PWM frequency
const int resolution = 8;  // PWM resolution (8-bit)
const int channel1 = 0;    // PWM channel for motor 1
const int channel2 = 1;    // PWM channel for motor 2
int dutyCycle = 0;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, data, len);
        if (error) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return;
        }
        const char *direction = doc["direction"].as<const char*>();
        double x = doc["x"].as<double>();
        double y = doc["y"].as<double>();

        // In "motor.ino"
        controlMotor(direction, x, y);
    }

}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebSocket()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void initWifi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    if (WiFi.softAPConfig(local_IP, gateway, subnet))
    {
        Serial.println("AP Config Ready");
    }
    else
    {
        Serial.println("AP Config Failed!");
    }

    if (WiFi.softAP(ssid_AP, password_AP))
    {
        Serial.println("AP Started");
        Serial.println("IP Address: " + WiFi.softAPIP().toString());
    }
    else
    {
        Serial.println("AP Failed to Start!");
    }
}

void setup()
{
    Serial.begin(115200);
    if (!SPIFFS.begin(true))
    {
        Serial.println("Failed to mount SPIFFS");
        return;
    }

    /* Motor Setup */

    // Motor pin setup
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(motor2Pin1, OUTPUT);
    pinMode(motor2Pin2, OUTPUT);

    // PWM setup
    // Configure PWM Pins
    ledcAttach(enable1Pin, freq, resolution);
    ledcAttach(enable2Pin, freq, resolution);

    // Initialize PWM
    ledcWrite(channel1, 0);
    ledcWrite(channel2, 0);

    /* Motor Setup END */

    initWifi();

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.begin();

    initWebSocket();
}

void loop()
{
    ws.cleanupClients();
}
