#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "motor.h";

const char *ssid_AP = "ECHO-AP";
const char *password_AP = "ourbread";
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String message = String((char *)data);

        if (message.startsWith("joystick:"))
        {
            String joystickData = message.substring(9);
            int commaIndex = joystickData.indexOf(',');
            if (commaIndex != -1)
            {
                String direction = joystickData.substring(0, commaIndex);
                String distance = joystickData.substring(commaIndex + 1);
                const char *directionCStr = direction.c_str();  // Conversion to const char*

                int distanceValue = distance.toInt();

                controlMotor(directionCStr, distanceValue);
            }
        }
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

    initWifi();
    setupMotorPins();

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.begin();

    initWebSocket();
}

void loop()
{
    ws.cleanupClients();
}
