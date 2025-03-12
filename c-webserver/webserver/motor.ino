void controlMotor(const char* direction, double x, double y)
{
    if (strcmp(direction, "Top") == 0)
    {
        handleForward(x, y);
    }
    else if (strcmp(direction, "Bottom") == 0)
    {
        handleBackward(x, y);
    }
    else if (strcmp(direction, "Left") == 0)
    {
        handleLeft(x, y);
    }
    else if (strcmp(direction, "Right") == 0)
    {
        handleRight(x, y);
    } 
    else if (strcmp(direction, "RightTop") == 0) 
    {
        handleForwardRight(x, y);
    } 
    else if (strcmp(direction, "TopLeft") == 0) 
    {
        handleForwardLeft(x, y);

    }
    else if (strcmp(direction, "LeftBottom") == 0) 
    {   
        handleBackwardLeft(x, y);
    }
    else if (strcmp(direction, "BottomRight") == 0) 
    {
        handleBackwardRight(x, y);
    }
    else if (strcmp(direction, "Stop") == 0)
    {
        handleStop(x, y);
    }
    else
    {
        Serial.println("ERROR: Invalid direction");
    }
}

void handleForward(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD, [%.2f, %.2f]\n", x, y);

}

void handleForwardRight(double x, double y) {
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD-RIGHT, [%.2f, %.2f]\n", x, y);
}

void handleForwardLeft(double x, double y) {
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD-LEFT, [%.2f, %.2f]\n", x, y);
}

void handleLeft(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: LEFT, [%.2f, %.2f]\n", x, y);
 
}

void handleStop(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: STOP, [%.2f, %.2f]\n", x, y);
    
}

void handleRight(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: RIGHT, [%.2f, %.2f]\n", x, y);
   
}

void handleBackward(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD, [%.2f, %.2f]\n", x, y);
    
}

void handleBackwardLeft(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD-LEFT, [%.2f, %.2f]\n", x, y);
    
}

void handleBackwardRight(double x, double y)
{
    Serial.printf("[ACTIVE] DRIVE Direction: BACKWARD-RIGHT, [%.2f, %.2f]\n", x, y);
    
}