void controlMotor(const char* direction, int distance)
{
    if (strcmp(direction, "Top") == 0)
    {
        handleForward(distance);
    }
    else if (strcmp(direction, "Bottom") == 0)
    {
        handleBackward(distance);
    }
    else if (strcmp(direction, "Left") == 0)
    {
        handleLeft(distance);
    }
    else if (strcmp(direction, "Right") == 0)
    {
        handleRight(distance);
    } 
    else if (strcmp(direction, "RightTop") == 0) 
    {
        handleForwardRight(distance);
    } 
    else if (strcmp(direction, "TopLeft") == 0) 
    {
        handleForwardLeft(distance);

    }
    else if (strcmp(direction, "LeftBottom") == 0) 
    {   
        handleBackwardLeft(distance);
    }
    else if (strcmp(direction, "BottomRight") == 0) 
    {
        handleBackwardRight(distance);
    }
    else if (strcmp(direction, "STOP") == 0 || strcmp(direction, "Center") == 0)
    {
        handleStop(distance);
    }
    else
    {
        Serial.println("ERROR: Invalid direction");
    }
}

void handleForward(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD, Distance: %d\n", distance);

}

void handleForwardRight(int distance) {
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD-RIGHT, Distance: %d\n", distance);
}

void handleForwardLeft(int distance) {
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD-LEFT, Distance: %d\n", distance);
}

void handleLeft(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: LEFT, Distance: %d\n", distance);
 
}

void handleStop(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: STOP, Distance: %d\n", distance);
    
}

void handleRight(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: RIGHT, Distance: %d\n", distance);
   
}

void handleBackward(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD, Distance: %d\n", distance);
    
}

void handleBackwardLeft(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD-LEFT, Distance: %d\n", distance);
    
}

void handleBackwardRight(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD-RIGHT, Distance: %d\n", distance);
    
}