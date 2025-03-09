#include "motor.h"
#include <Arduino.h>

// Motor Pins
int motor1Pin1 = 21;
int motor1Pin2 = 22;
int enable1Pin = 23;
int motor2Pin1 = 5;
int motor2Pin2 = 18;
int enable2Pin = 19;

// PWM settings
const int freq = 30000;   // PWM frequency
const int resolution = 8; // PWM resolution (8-bit)
const int channel1 = 0;   // PWM channel for motor 1
const int channel2 = 1;   // PWM channel for motor 2
int dutyCycle = 0;

void setupMotorPins() {
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    pinMode(motor2Pin1, OUTPUT);
    pinMode(motor2Pin2, OUTPUT);
    pinMode(enable2Pin, OUTPUT);

    ledcSetup(channel1, freq, resolution);
    ledcSetup(channel2, freq, resolution);

    ledcAttachPin(enable1Pin, channel1);
    ledcAttachPin(enable2Pin, channel2);
}

void handleForward(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD, Distance: %d\n", distance);
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
}

void handleLeft(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: LEFT, Distance: %d\n", distance);
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
}

void handleStop(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: STOP, Distance: %d\n", distance);
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
}

void handleRight(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: RIGHT, Distance: %d\n", distance);
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
}

void handleBackward(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD, Distance: %d\n", distance);
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
}

void controlMotor(const char* direction, int distance)
{
    if (strcmp(direction, "FORWARD") == 0)
    {
        handleForward(distance);
    }
    else if (strcmp(direction, "BACKWARD") == 0)
    {
        handleBackward(distance);
    }
    else if (strcmp(direction, "LEFT") == 0)
    {
        handleLeft(distance);
    }
    else if (strcmp(direction, "RIGHT") == 0)
    {
        handleRight(distance);
    }
    else if (strcmp(direction, "STOP") == 0)
    {
        handleStop(distance);
    }
    else
    {
        Serial.println("ERROR: Invalid direction");
    }
}
