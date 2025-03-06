#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>

const char *ssid_AP = "ECHO-AP";
const char *password_AP = "ourbread";
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
WebSocketsServer webSocket(81); // WebSocket server on port 81

void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_TEXT)
    {
        String message = String((char *)payload);
        Serial.println("Received: " + message);

        // Parse joystick data
        if (message.startsWith("joystick:"))
        {
            String data = message.substring(9);
            int commaIndex = data.indexOf(',');
            int x = data.substring(0, commaIndex).toInt();
            int y = data.substring(commaIndex + 1).toInt();
            Serial.printf("Joystick X: %d, Y: %d\n", x, y);
        }
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

    WiFi.softAP(ssid_AP, password_AP);
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.begin();

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void loop()
{
    webSocket.loop();
}
