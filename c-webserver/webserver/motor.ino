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

void handleForward(int distance)
{
    Serial.printf("[ACTIVE] DRIVE Direction: FORWARD, Distance: %d\n", distance);

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
