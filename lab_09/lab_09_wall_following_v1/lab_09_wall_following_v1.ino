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
const int SET_POINT = 20;
const float KP = 4;

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
bool doLineFollowing = false;
bool doWallFollowing = true;
bool doObjectDetection = false;


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
    followWall();

    // if (doLineFollowing)
    // {
    //     followLine();
    // }
    // else if (doWallFollowing) 
    // {
    //     followWall();
    // }
    // else if (doObjectDetection) 
    // {
    //     detectObject();
    // }

    // if (Serial.available() > 0)
    // {
    //     char command = Serial.read();
    //     switch (command)
    //     {
    //         case 'l':
    //             doLineFollowing = true;
    //             doWallFollowing = false;
    //             doObjectDetection = false;
    //             delay(1000);
    //             break;
    //         case 'w':
    //             doLineFollowing = false;
    //             doWallFollowing = true;
    //             doObjectDetection = false;
    //             delay(1000);
    //             break;
    //         case 'o':
    //             doLineFollowing = false;
    //             doWallFollowing = false;
    //             doObjectDetection = true;
    //             delay(1000);
    //             break;
    //         default:
    //             break;
    //     }
    // }
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
    if (speed > 120) speed = 120;
    if (speed < 60) speed = 60;

    analogWrite(motorB, speed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed * 0.80);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    delay(DELAY_TIME);
    stop();
}

void forwardRight(int speed)
{
    if (speed > 120) speed = 120;
    if (speed < 60) speed = 60;

    analogWrite(motorB, speed * 0.80);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    analogWrite(motorA, speed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    delay(DELAY_TIME);
    stop();
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



// Wall-following functions
void followWall()
{
    int distance = triggerSensor(triggerPinLeft, echoPinLeft);
    int output = wallFollowingPid(distance);

    int distanceRight = triggerSensor(triggerPinFront, echoPinFront);
    myservo.write(0); // look right 

    // if (distance > 75 && distanceRight > 75)
    // {
    //     Serial.println("Switching to object detection");
    //     doWallFollowing = false;
    //     doObjectDetection = true;
    //     stop();
    //     delay(500);
    //     return;
    // }

    if (output < 0) forwardLeft(output);
    else if (output > 0) forwardRight(-output);

    Serial.println(output);
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

    return output;
}