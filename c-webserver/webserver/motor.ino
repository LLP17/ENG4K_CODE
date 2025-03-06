// #include <ArduinoJson.h>

// // Motor Pins
// int motor1Pin1 = 21; 
// int motor1Pin2 = 22; 
// int enable1Pin = 23;
// int motor2Pin1 = 5; 
// int motor2Pin2 = 18; 
// int enable2Pin = 19;

// // PWM settings
// const int freq = 30000;    // PWM frequency
// const int resolution = 8;  // PWM resolution (8-bit)
// const int channel1 = 0;    // PWM channel for motor 1
// const int channel2 = 1;    // PWM channel for motor 2
// int dutyCycle = 0;

// void handleForward() {
//     Serial.println("Forward");
//     digitalWrite(motor1Pin1, HIGH);
//     digitalWrite(motor1Pin2, LOW); 
//     digitalWrite(motor2Pin1, HIGH);
//     digitalWrite(motor2Pin2, LOW);
//     server.send(200);
//   }
  
//   void handleLeft() {
//     Serial.println("Left");
//     digitalWrite(motor1Pin1, LOW); 
//     digitalWrite(motor1Pin2, LOW); 
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, HIGH);
//     server.send(200);
//   }
  
//   void handleStop() {
//     Serial.println("Stop");
//     digitalWrite(motor1Pin1, LOW); 
//     digitalWrite(motor1Pin2, LOW); 
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, LOW);   
//     server.send(200);
//   }
  
//   void handleRight() {
//     Serial.println("Right");
//     digitalWrite(motor1Pin1, LOW); 
//     digitalWrite(motor1Pin2, HIGH); 
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, LOW);    
//     server.send(200);
//   }
  
//   void handleReverse() {
//     Serial.println("Reverse");
//     digitalWrite(motor1Pin1, LOW);
//     digitalWrite(motor1Pin2, HIGH); 
//     digitalWrite(motor2Pin1, LOW);
//     digitalWrite(motor2Pin2, HIGH);          
//     server.send(200);
//   }
  
//   void handleSpeed() {
//     if (server.hasArg("value")) {
//       valueString = server.arg("value");
//       int value = valueString.toInt();
//       if (value == 0) {
//         ledcWrite(enable1Pin, 0);
//         ledcWrite(enable2Pin, 0);
//         digitalWrite(motor1Pin1, LOW); 
//         digitalWrite(motor1Pin2, LOW); 
//         digitalWrite(motor2Pin1, LOW);
//         digitalWrite(motor2Pin2, LOW);   
//       } else { 
//         dutyCycle = map(value, 25, 100, 200, 255);
//         ledcWrite(enable1Pin, dutyCycle);
//         ledcWrite(enable2Pin, dutyCycle);
//         Serial.println("Motor speed set to " + String(value));
//       }
//     }
//     server.send(200);
//   }