#include <WiFi.h>
#include <WebServer.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

uint8_t BATTERT_PIN = 10; 
uint8_t MOTOR1A_PIN = 4;
uint8_t MOTOR1B_PIN = 5; 

uint8_t MOTOR2A_PIN = 6;
uint8_t MOTOR2B_PIN = 7; 

uint8_t MOTOR1EN_PIN = 4;
uint8_t MOTOR2EN_PIN = 5; 

void setup() {
  Serial.begin(115200);
  pinMode(BATTERY_PIN,INPUT); 
  pinMode(MOTOR1A_PIN, OUTPUT);
  pinMode(MOTOR2A_PIN, OUTPUT);
  pinMode(MOTOR1B_PIN, OUTPUT);
  pinMode(MOTOR2B_PIN, OUTPUT);
  pinMode(MOTOR1EN_PIN, OUTPUT);
  pinMode(MOTOR2EN_PIN, OUTPUT);

  analogWrite(MOTOR1EN_PIN,255);
  analogWrite(MOTOR2EN_PIN,255);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/forward", handle_led1on);
  server.on("/backward", handle_led1off);
  server.on("/left", handle_led2on);
  server.on("/right", handle_led2off);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  server.handleClient();
}

float get_voltage(){ 
  int reading = analogRead(BATTERY_PIN); 
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
  Serial.println(getVoltage());
  server.send(200,"text/html", SendHTMML(getVoltage()));
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
  Serial.println(getVoltage());
  server.send(200,"text/html", SendHTMML(getVoltage()));
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
  Serial.println(getVoltage());
  server.send(200,"text/html", SendHTMML(getVoltage()));
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
  Serial.println(getVoltage());
  server.send(200,"text/html", SendHTMML(getVoltage()));
}

void handle_OnConnect() {
  Serial.println(getVoltage());
  server.send(200, "text/html", SendHTML(getVoltage())); 
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
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

