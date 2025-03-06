#include <WiFi.h>
#include <ESPAsyncWebServer.h>  // Include the Async Web Server library

const char *ssid_AP = "ESP32-AP";         // Name of the access point
const char *password_AP = "12345678";     // Password for the access point

IPAddress local_IP(192, 168, 1, 100);     // ESP32 IP address
IPAddress gateway(192, 168, 1, 10);       // Gateway IP
IPAddress subnet(255, 255, 255, 0);       // Subnet mask

AsyncWebServer server(80);  // Create a web server on port 80

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Server</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
    h1 { color: #333; }
    button { padding: 10px 20px; margin: 10px; }
  </style>
</head>
<body>
  <h1>Welcome to ESP32 Web Server!</h1>
  <button onclick="alert('Button Clicked!')">Click Me</button>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(2000);

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

  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlPage);
  });

  // Start the server
  server.begin();
  Serial.println("Server started!");
}

void loop() {
  // No code needed here for this example
}
