const int SPEED_MAX = 170;
typedef enum motorState {
  forward_t,
  backward_t,
  stop_t
};

int maxSpeed = SPEED_MAX;
int leftMotorState = stop_t;
int rightMotorState = stop_t;

void controlMotor(const char* direction, double x, double y)
{
    int xInt = int(x);
    int yInt = int(y);

    int leftSpeed = constrain(map(yInt + xInt, -100, 100, -255, 255), -255, 255);
    int rightSpeed = constrain(map(yInt - xInt, -100, 100, -255, 255), -255, 255);

    setMotorSpeeds(leftSpeed, rightSpeed);
    Serial.println("calling 'controlMotor'");

}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Left motor
  if (leftSpeed > 0) {
    Serial.println("leftSpeed > 0");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    leftMotorState = forward_t;
  } else if (leftSpeed < 0) {
    Serial.println("leftSpeed < 0");
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    leftSpeed = -leftSpeed;
    leftMotorState = backward_t;
  } else {
    Serial.println("leftSpeed == 0");
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    leftMotorState = stop_t;
  }

  // Right motor
  if (rightSpeed > 0) {
    Serial.println("rightSpeed > 0");
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    rightMotorState = forward_t;
  } else if (rightSpeed < 0) {
    Serial.println("rightSpeed < 0");
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    rightSpeed = -rightSpeed;
    rightMotorState = backward_t;
  } else {
    Serial.println("rightSpeed == 0");
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    rightMotorState = stop_t;
  }

  if (leftMotorState != rightMotorState) {
    maxSpeed = 110;
  } else {
    maxSpeed = SPEED_MAX;
  }

  // Set PWM values
  ledcWrite(enable1Pin, min(leftSpeed, maxSpeed));
  ledcWrite(enable2Pin, min(rightSpeed, maxSpeed));
}
