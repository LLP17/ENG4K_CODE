#include <WiFi.h>
#include <WiFiAP.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "yourAp";
const char* password = "yourPassword";

// WebServer on port 80
AsyncWebServer server(80);
// Websocket endpoint
AsyncWebSocket ws("/ws"); 

// Motor Pins
int motor1Pin1 = 21; 
int motor1Pin2 = 22; 
int enable1Pin = 23;
int motor2Pin1 = 5; 
int motor2Pin2 = 18; 
int enable2Pin = 19;

// PWM settings
const int freq = 30000;    // PWM frequency
const int resolution = 8;  // PWM resolution (8-bit)
const int channel1 = 0;    // PWM channel for motor 1
const int channel2 = 1;    // PWM channel for motor 2
int dutyCycle = 0;

// HTML page with joystick
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0; padding: 0; }
    canvas { touch-action: none; background: #ccc; margin: 20px auto; display: block; }
  </style>
  <script>
    // Web Socket Initialization
    let gateway = `ws://${window.location.hostname}/ws`;
    let ws;

    window.addEventListener('load', onLoad);

    function onLoad(event) {
      initWebSocket();
    }
    
    function initWebSocket() {
      ws = new WebSocket(gateway);
    }
    
    let joystick = { x: 0, y: 0 };
    let centerX = 150, centerY = 150, maxRadius = 100;

    function updateJoystick(event) {
      const rect = event.target.getBoundingClientRect();
      const x = event.touches ? event.touches[0].clientX : event.clientX;
      const y = event.touches ? event.touches[0].clientY : event.clientY;

      const offsetX = x - rect.left - centerX;
      const offsetY = y - rect.top - centerY;

      const distance = Math.min(Math.sqrt(offsetX**2 + offsetY**2), maxRadius);
      const angle = Math.atan2(offsetY, offsetX);

      joystick.x = Math.round((distance * Math.cos(angle)) / maxRadius * 100);
      joystick.y = Math.round((distance * Math.sin(angle)) / maxRadius * 100);

      sendJoystickData(joystick);
      drawJoystick();
    }

    function resetJoystick() {
      joystick.x = 0;
      joystick.y = 0;
      sendJoystickData(0, 0);
      drawJoystick();
    }

    function sendJoystickData(joystick) {
      // x, y arguments are keys in a JSON object with corresponding x, y motor speed values
      if (ws && ws.readyState == WebSocket.OPEN) {
        ws.send(JSON.stringify(joystick));
      }
    }

    function drawJoystick() {
      const canvas = document.getElementById('joystick');
      const ctx = canvas.getContext('2d');

      ctx.clearRect(0, 0, canvas.width, canvas.height);

      // Draw joystick base
      ctx.beginPath();
      ctx.arc(centerX, centerY, maxRadius, 0, Math.PI * 2);
      ctx.fillStyle = '#888';
      ctx.fill();

      // Draw joystick handle
      ctx.beginPath();
      ctx.arc(centerX + joystick.x, centerY + joystick.y, 20, 0, Math.PI * 2);
      ctx.fillStyle = '#444';
      ctx.fill();
    }

    document.addEventListener('DOMContentLoaded', () => {
      initWebSocket();
      const canvas = document.getElementById('joystick');
      canvas.addEventListener('mousedown', updateJoystick);
      canvas.addEventListener('mousemove', (e) => e.buttons && updateJoystick(e));
      canvas.addEventListener('mouseup', resetJoystick);
      canvas.addEventListener('mouseleave', resetJoystick);
      canvas.addEventListener('touchstart', updateJoystick);
      canvas.addEventListener('touchmove', updateJoystick);
      canvas.addEventListener('touchend', resetJoystick);
      drawJoystick();
    });
  </script>
</head>
<body>
  <h1>ECHO-ALPHA</h1>
  <canvas id="joystick" width="300" height="300"></canvas>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleJoystick(int x, int y) {
  // Calculate motor speeds and directions
    int leftSpeed = constrain(map(y + x, -100, 100, -255, 255), -255, 255);
    int rightSpeed = constrain(map(y - x, -100, 100, -255, 255), -255, 255);

    setMotorSpeeds(leftSpeed, rightSpeed);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Left motor
  if (leftSpeed > 0) {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
  } else if (leftSpeed < 0) {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    leftSpeed = -leftSpeed;
  } else {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
  }

  // Right motor
  if (rightSpeed > 0) {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
  } else if (rightSpeed < 0) {
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    rightSpeed = -rightSpeed;
  } else {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
  }

  // Set PWM values
  ledcWrite(enable1Pin, leftSpeed);
  ledcWrite(enable2Pin, rightSpeed);
}

// Helper function for AwsEventType->WS_EVT_DATA
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    int x = doc["x"];
    int y = doc["y"];
    handleJoystick(x, y); // Pass joystick data to handler
  }
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      //data is received from the client (json)
      handleWebSocketMessage(arg, data, len);
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
  
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

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

  // Wi-Fi setup
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  // Web Socket Setup
  initWebSocket();

  // Start server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html, html");
  });
  server.begin();
}

void loop() {
  server.handleClient();
  ws.cleanupClients();
}
