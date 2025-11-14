// Lab 09 - Model Tournament

#include <Servo.h>

Servo myservo;

// Motor pins & speed
const int motorApin1 = 3, motorApin2 = 2; // left motor
const int motorBpin1 = 7, motorBpin2 = 4; // right motor
const int motorA = 5, motorB = 6;
const int DEFAULT_SPEED = 100;
const int START_UP_SPEED = 100;

// Line-following
const int irSensorPinCentre = A0;
const int irSensorPinLeft = A1;
const int irSensorPinRight = A2;

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
void followLine()
{
    int sensorValueCentre = digitalRead(irSensorPinCentre);
    int sensorValueLeft = digitalRead(irSensorPinLeft);
    int sensorValueRight = digitalRead(irSensorPinRight);
    
    // long distance = triggerSensor(triggerPinLeft, echoPinLeft);

    // if (sensorValueCentre == HIGH && sensorValueLeft == HIGH && sensorValueRight == HIGH &&
    //     distance <= 20)
    // {
    //     Serial.println("Switching to wall following");
    //     doLineFollowing = false;
    //     doWallFollowing = true;
    //     return;
    // }
    if (sensorValueLeft == LOW && sensorValueRight == HIGH ||
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



// Motor functions
void forward(int speed)
{
  analogWrite(motorB, speed * 0.75);
  digitalWrite(motorBpin1, HIGH);
  digitalWrite(motorBpin2, LOW);

  analogWrite(motorA, speed * 0.75);
  digitalWrite(motorApin1, HIGH);
  digitalWrite(motorApin2, LOW);

  delay(50);
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

    delay(50);
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

    delay(50);
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

    delay(100);
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
    followLine();
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