#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "ECHO-ALPHA";
const char* password = "aLpHaEcHo1";

// WebServer on port 80
WebServer server(80);

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

      sendJoystickData(joystick.x, joystick.y);
      drawJoystick();
    }

    function resetJoystick() {
      joystick.x = 0;
      joystick.y = 0;
      sendJoystickData(0, 0);
      drawJoystick();
    }

    function sendJoystickData(x, y) {
      fetch(`/joystick?x=${x}&y=${y}`);
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

void handleJoystick() {
  if (server.hasArg("x") && server.hasArg("y")) {
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();

    // Calculate motor speeds and directions
    int leftSpeed = constrain(map(y + x, -100, 100, -255, 255), -255, 255);
    int rightSpeed = constrain(map(y - x, -100, 100, -255, 255), -255, 255);

    setMotorSpeeds(leftSpeed, rightSpeed);
  }
  server.send(200);
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

  // Routes
  server.on("/", handleRoot);
  server.on("/joystick", handleJoystick);

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}
