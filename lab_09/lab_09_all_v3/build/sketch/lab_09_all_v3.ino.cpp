#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
// Lab 09 - Model Tournament

#include <Servo.h>

Servo myservo;

// Motor constants
const int motorApin1 = 7, motorApin2 = 4; // left motor
const int motorBpin1 = 3, motorBpin2 = 2; // right motor
const int motorA = 5, motorB = 6;

const int MIN_SPEED = 70;
const int DEFAULT_SPEED = 120;

// Line-following
const int irSensorPinCentre = A0;
const int irSensorPinLeft = A1;
const int irSensorPinRight = A2;
const int DELAY_TIME = 100;

// Wall-following
const int triggerPinLeft = 10, echoPinLeft = 11;
const float SET_POINT = 15.0; 
const float Kp = 0.3;
const float Ki = 0.0;
const float Kd = 0.3; 
float lastError = 0; // Store the error from the previous loop iteration
const float DT = 0.030; // Time step in seconds (30ms / 1000)

long duration;
const int MAX_DISTANCE_WF = 25;
const int MIN_DISTANCE_WF = 3;

// Object detection
const int triggerPinFront = 12, echoPinFront = 13;
const int servoPin = 9;
const float MAX_DISTANCE_OF = 100.0;
const float MIN_DISTANCE_OD = 5.0;
const int CENTRE_ANGLE = 90;

int servoPosition = 90;
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
#line 57 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void followLine();
#line 97 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void followWall();
#line 130 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
float triggerSensor(int triggerPin, int echoPin);
#line 154 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
int pidWallFollowing(float distance);
#line 174 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void detectObject();
#line 183 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void sweepRecord();
#line 206 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
int findBestAngle();
#line 236 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
int moveTowardAnglePid(int angle);
#line 251 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void forward(int speed);
#line 265 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void forwardWallFollowing(int leftSpeed, int rightSpeed);
#line 275 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void backwardWallFollowing(int leftSpeed, int rightSpeed);
#line 285 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void spin();
#line 295 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void forwardLeft(int speed);
#line 306 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void forwardRight(int speed);
#line 317 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void left(int speed);
#line 331 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void right(int speed);
#line 345 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void stop();
#line 358 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void setup();
#line 388 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
void loop();
#line 57 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_all_v3\\lab_09_all_v3.ino"
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
    else if (sensorValueRight == LOW && sensorValueLeft == LOW && sensorValueCentre == LOW) 
    { 
        backwardWallFollowing(DEFAULT_SPEED, DEFAULT_SPEED);
        delay(50);
    }
    else
    {
        forward(DEFAULT_SPEED);
    }
}



// Wall-following functions
void followWall()
{
    float distanceLeft = triggerSensor(triggerPinLeft, echoPinLeft);
    float distanceFront = triggerSensor(triggerPinFront, echoPinFront);

    int sensorValueCentre = digitalRead(irSensorPinCentre);
    int sensorValueLeft = digitalRead(irSensorPinLeft);
    int sensorValueRight = digitalRead(irSensorPinRight);

    forwardWallFollowing(100, (100 * 1.9));

    if (distanceFront < MIN_DISTANCE_WF + 2) {
        backwardWallFollowing(MIN_SPEED, MIN_SPEED);
        delay(300);
        spin();
        delay(50);
        lastError = 0;
    }

    if (distanceLeft > (MAX_DISTANCE_WF - 5) &&
        distanceFront > ( MIN_DISTANCE_WF + 5) &&
        (sensorValueLeft == LOW && sensorValueCentre == LOW) ||
        (sensorValueRight == LOW && sensorValueCentre == LOW)) {

            Serial.println("Switching to object detection");
            doWallFollowing = false;
            doObjectDetection = true;
            stop();
            delay(500);
            return;
    }
}

float triggerSensor(int triggerPin, int echoPin) {
    float totalDistance = 0;
    const int NUM_READINGS = 3; 

    for (int i = 0; i < NUM_READINGS; i++) {
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(5);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);

        long duration = pulseIn(echoPin, HIGH);
        float distance = (duration / 2.0) / 29.1;
        totalDistance += distance;
        delayMicroseconds(50);
    }
    
    float distance = totalDistance / NUM_READINGS;

    if (distance > MAX_DISTANCE_WF) distance = MAX_DISTANCE_WF;

    return distance;
}

int pidWallFollowing(float distance) {
    float error = distance - SET_POINT;

    float P_out = Kp * error;

    float derivative = (error - lastError) / DT; 
    float D_out = Kd * derivative;

    lastError = error; 

    float output_float = P_out + D_out;

    Serial.println(output_float);

    return (int)output_float;
}



// Object detection functions
void detectObject()
{
    forwardWallFollowing(100, (100 * 1.13));

    // sweepRecord();
    // int angle = findBestAngle();
    // moveTowardAnglePid(angle);
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
        beliefs[i] = beliefs[i] / MAX_DISTANCE_OF;
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

int moveTowardAnglePid(int angle)
{
    int error = angle - CENTRE_ANGLE; // Center position is 90 degrees
    int output = Kp * error;

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

void forwardWallFollowing(int leftSpeed, int rightSpeed) {
    analogWrite(motorA, leftSpeed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, rightSpeed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);
}

void backwardWallFollowing(int leftSpeed, int rightSpeed) {
    analogWrite(motorA, leftSpeed * 2);
    digitalWrite(motorApin1, LOW);
    digitalWrite(motorApin2, HIGH);

    analogWrite(motorB, rightSpeed * 2);
    digitalWrite(motorBpin1, LOW);
    digitalWrite(motorBpin2, HIGH);
}

void spin() {
    analogWrite(motorA, DEFAULT_SPEED * 2);
    digitalWrite(motorApin1, LOW);
    digitalWrite(motorApin2, HIGH);

    analogWrite(motorB, DEFAULT_SPEED * 2);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);
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
    myservo.write(90);

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
