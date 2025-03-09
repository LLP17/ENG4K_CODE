#ifndef MOTOR_H
#define MOTOR_H

void handleForward(int distance);
void handleLeft(int distance);
void handleStop(int distance);
void handleRight(int distance);
void handleBackward(int distance);
void controlMotor(const char* direction, int distance);

#endif