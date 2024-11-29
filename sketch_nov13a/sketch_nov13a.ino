#include <WiFi.h>
#include <WebServer.h>

/* Put your SSID & Password */
const char* ssid = "LLPESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

uint8_t BATTERY_PIN = 10; 
uint8_t MOTOR1A = 22;
uint8_t MOTOR1B = 23; 

uint8_t MOTOR2A = 25;
uint8_t MOTOR2B = 32; 

uint8_t MOTOR1EN = 4;
uint8_t MOTOR2EN = 26; 

void setup() {
  Serial.begin(115200);
  pinMode(BATTERY_PIN,INPUT); 
  pinMode(MOTOR1A, OUTPUT);
  pinMode(MOTOR2A, OUTPUT);
  pinMode(MOTOR1B, OUTPUT);
  pinMode(MOTOR2B, OUTPUT);
  pinMode(MOTOR1EN, OUTPUT);
  pinMode(MOTOR2EN, OUTPUT);

  analogWrite(MOTOR1EN,255);
  analogWrite(MOTOR2EN,255);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/forward", handle_forward);
  server.on("/backward", handle_backward);
  server.on("/left", handle_left);
  server.on("/right", handle_right);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  server.handleClient();
}

float get_voltage(){ 
  uint16_t reading = analogRead(BATTERY_PIN); 
  float voltage = reading * (3.3/1023); 
  return voltage; 
}


void handle_forward(){ 
  analogWrite(MOTOR1A, 255); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 255); 
  analogWrite(MOTOR2B, 0); 
  delay(10);
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 0); 
  Serial.println(get_voltage());
  server.send(200,"text/html", SendHTML(get_voltage()));
}

void handle_backward(){ 
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 255); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 255); 
  delay(10);
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 0); 
  Serial.println(get_voltage());
  server.send(200,"text/html", SendHTML(get_voltage()));
}
void handle_left(){ 
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 255); 
  analogWrite(MOTOR2A, 255); 
  analogWrite(MOTOR2B, 0); 
  delay(10);
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 0); 
  Serial.println(get_voltage());
  server.send(200,"text/html", SendHTML(get_voltage()));
}
void handle_right(){ 
  analogWrite(MOTOR1A, 255); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 255); 
  delay(10);
  analogWrite(MOTOR1A, 0); 
  analogWrite(MOTOR1B, 0); 
  analogWrite(MOTOR2A, 0); 
  analogWrite(MOTOR2B, 0); 
  Serial.println(get_voltage());
  server.send(200,"text/html", SendHTML(get_voltage()));
}

void handle_OnConnect() {
  Serial.println(get_voltage());
  server.send(200, "text/html", SendHTML(get_voltage())); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float voltage){
  // Todo 
  // Create Buttons that will link to their correspoding urls: 
  // "/forward", "/backward", "/left" ,"/right"
  // Add style to the buttons so that on hover they will change opacity if time permits
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Robot Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
  ptr +="<p>LED1 Status: ON</p><a class=\"button button-on\" href=\"/forward\">OFF</a>\n";
  ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/backward\">ON</a>\n";
  ptr +="<p>LED2 Status: ON</p><a class=\"button button-on\" href=\"/left\">OFF</a>\n";
  ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/right\">ON</a>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

