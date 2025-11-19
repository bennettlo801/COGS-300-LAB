#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
// Lab 09 - Model Tournament

#include <Servo.h>

Servo myservo;

// Motor pins & speed
const int motorApin1 = 3, motorApin2 = 2; // left motor
const int motorBpin1 = 7, motorBpin2 = 4; // right motor
const int motorA = 5, motorB = 6;
const int DEFAULT_SPEED = 100;

// Line-following
const int irSensorPinCentre = A0;
const int irSensorPinLeft = A1;
const int irSensorPinRight = A2;
const int DELAY_TIME = 150;

// Wall-following
const int triggerPinLeft = 10, echoPinLeft = 11;
const int SET_POINT = 10;
const float KP = 1.0;

// Object detection
const int triggerPinFront = 12, echoPinFront = 13;
const int servoPin = 9;
const float MAX_DISTANCE = 100.0;
const float MIN_DISTANCE = 5.0;
const int CENTRE_ANGLE = 90;

int servoPosition = 0;
int incrementServo = 6;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 100;

const int  NUM_POSITIONS = 36;
float beliefs[NUM_POSITIONS];

// Behaviour constants
bool doLineFollowing = true;
bool doWallFollowing = false;
bool doObjectDetection = false;



// Line-following functions
#line 47 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void followLine();
#line 82 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void followWall();
#line 104 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
float triggerSensor(int triggerPin, int echoPin);
#line 121 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
int wallFollowingPid(long distance);
#line 134 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void detectObject();
#line 141 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void sweepRecord();
#line 164 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
int findBestAngle();
#line 194 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void moveTowardAnglePid(int angle);
#line 209 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void forward(int speed);
#line 223 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void forwardLeft(int speed);
#line 234 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void forwardRight(int speed);
#line 245 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void left(int speed);
#line 259 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void right(int speed);
#line 273 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void stop();
#line 288 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void setup();
#line 317 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void loop();
#line 47 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09.ino"
void followLine()
{
    int sensorValueCentre = digitalRead(irSensorPinCentre);
    int sensorValueLeft = digitalRead(irSensorPinLeft);
    int sensorValueRight = digitalRead(irSensorPinRight);
    
    long distance = triggerSensor(triggerPinLeft, echoPinLeft);

    if (sensorValueCentre == HIGH && sensorValueLeft == HIGH && sensorValueRight == HIGH &&
        distance <= 20)
    {
        Serial.println("Switching to wall following");
        doLineFollowing = false;
        doWallFollowing = true;
        return;
    }
    else if (sensorValueLeft == LOW && sensorValueRight == HIGH ||
             sensorValueLeft == LOW && sensorValueCentre == LOW)
    {
        left(DEFAULT_SPEED);
    }
    else if (sensorValueRight == LOW && sensorValueLeft == HIGH ||
             sensorValueRight == LOW && sensorValueCentre == LOW)
    {
        right(DEFAULT_SPEED);
    }
    else
    {
        forward(DEFAULT_SPEED);
    }
}



// Wall-following functions
void followWall()
{
    int distance = triggerSensor(triggerPinLeft, echoPinLeft);
    int output = wallFollowingPid(distance);

    int distanceRight = triggerSensor(triggerPinFront, echoPinFront);
    myservo.write(0); // look right 

    if (distance > 75 && distanceRight > 75)
    {
        Serial.println("Switching to object detection");
        doWallFollowing = false;
        doObjectDetection = true;
        stop();
        delay(500);
        return;
    }

    if (output > 0) forwardLeft(output);
    else if (output < 0) forwardRight(-output);
}

float triggerSensor(int triggerPin, int echoPin)
{
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 20000);
    float distance = (duration / 2) / 29.1;

    if (distance >= MAX_DISTANCE) distance = MAX_DISTANCE;
    else if (distance <= MIN_DISTANCE) distance = MIN_DISTANCE;

    return distance;
}

int wallFollowingPid(long distance)
{
    int error = SET_POINT - distance;
    int output = KP * error;

    if (output > 100) output = 100;

    return output;
}



// Object detection functions
void detectObject()
{
    sweepRecord();
    int angle = findBestAngle();
    moveTowardAnglePid(angle);
}

void sweepRecord()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= updateInterval)
    {
        lastUpdate = currentMillis;

        servoPosition += incrementServo;
        myservo.write(servoPosition);

        if (servoPosition % 5 == 0) {
            int index = servoPosition / 5;
            beliefs[index] = triggerSensor(triggerPinFront, echoPinFront);
        }

        if (servoPosition >= 180 || servoPosition <= 0)
        {
            incrementServo = -incrementServo;
        }
    }
}

int findBestAngle()
{
    // Normalize each belief
    for (int i = 0; i < NUM_POSITIONS; i++)
    {
        beliefs[i] = beliefs[i] / MAX_DISTANCE;
    }

    // Invert to likelihoods
    for (int i = 0; i < NUM_POSITIONS; i++)
    {
        beliefs[i] = 1.0 - beliefs[i];
    }

    // Find the index of the maximum belief
    int maxIndex = 0;
    float maxBelief = beliefs[0];

    for (int i = 1; i < NUM_POSITIONS; i++)
    {
        if (beliefs[i] > maxBelief)
        {
            maxBelief = beliefs[i];
            maxIndex = i;
        }
    }

    return maxIndex * (180 / NUM_POSITIONS);
}

void moveTowardAnglePid(int angle)
{
    int error = angle - CENTRE_ANGLE; // Center position is 90 degrees
    int output = KP * error;

    if (output > 0) right(output);
    else if (output < 0) left(output);
    else forward(output);

    return output;
}



// Motor control functions
void forward(int speed)
{
  analogWrite(motorB, speed * 0.75);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, speed * 0.75);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  delay(DELAY_TIME);
  stop();
}

void forwardLeft(int speed)
{
    analogWrite(motorB, speed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed * 0.75);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);
}

void forwardRight(int speed)
{
    analogWrite(motorB, speed * 0.75);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);
}

void left(int speed)
{
    analogWrite(motorB, speed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed * 0.25);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    delay(DELAY_TIME);
    stop();
}

void right(int speed)
{
    analogWrite(motorB, speed * 0.25);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    delay(DELAY_TIME);
    stop();
}

void stop()
{
    analogWrite(motorA, 0);
    digitalWrite(motorApin1, LOW);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, 0);
    digitalWrite(motorBpin1, LOW);
    digitalWrite(motorBpin2, LOW);

    delay(DELAY_TIME * 2);
}



void setup()
{
    // Initialize motor pins
    pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
    pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);
    pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

    // Initialize IR sensor pins
    pinMode(irSensorPinCentre, INPUT);
    pinMode(irSensorPinLeft, INPUT);
    pinMode(irSensorPinRight, INPUT);

    // Intialize servo
    myservo.attach(servoPin);

    // Initialize ultrasonic sensor pins
    pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
    pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

    // Initialize beliefs uniformly
    for (int i = 0; i < NUM_POSITIONS; i++)
    {
        beliefs[i] = 1.0 / NUM_POSITIONS;
    }

    Serial.begin(9600);
    delay(2000);
}

void loop()
{
    if (doLineFollowing)
    {
        followLine();
    }
    else if (doWallFollowing) 
    {
        followWall();
    }
    else if (doObjectDetection) 
    {
        detectObject();
    }

    if (Serial.available() > 0)
    {
        char command = Serial.read();
        switch (command)
        {
            case 'l':
                doLineFollowing = true;
                doWallFollowing = false;
                doObjectDetection = false;
                delay(1000);
                break;
            case 'w':
                doLineFollowing = false;
                doWallFollowing = true;
                doObjectDetection = false;
                delay(1000);
                break;
            case 'o':
                doLineFollowing = false;
                doWallFollowing = false;
                doObjectDetection = true;
                delay(1000);
                break;
            default:
                break;
        }
    }
}
