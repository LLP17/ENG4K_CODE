void controlMotor(const char* direction, double x, double y)
{
    int xInt = int(x);
    int yInt = int(y);

    int leftSpeed = constrain(map(yInt + xInt, -80, 80, -255, 255), -255, 255);
    int rightSpeed = constrain(map(yInt - xInt, -80, 80, -255, 255), -255, 255);

    setMotorSpeeds(leftSpeed, rightSpeed);
    Serial.println("calling 'controlMotor'");

}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Left motor
  if (leftSpeed > 0) {
    Serial.println("leftSpeed > 0");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
  } else if (leftSpeed < 0) {
    Serial.println("leftSpeed < 0");
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    leftSpeed = -leftSpeed;
  } else {
    Serial.println("leftSpeed == 0");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
  }

  // Right motor
  if (rightSpeed > 0) {
    Serial.println("rightSpeed > 0");
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
  } else if (rightSpeed < 0) {
    Serial.println("rightSpeed < 0");
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    rightSpeed = -rightSpeed;
  } else {
    Serial.println("rightSpeed == 0");
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
  }

  // Set PWM values
  ledcWrite(enable1Pin, leftSpeed);
  ledcWrite(enable2Pin, rightSpeed);
}
