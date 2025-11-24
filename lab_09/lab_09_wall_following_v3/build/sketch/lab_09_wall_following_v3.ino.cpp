#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
// Motor constants
const int motorApin1 = 7, motorApin2 = 4; // left motor
const int motorBpin1 = 3, motorBpin2 = 2; // right motor
const int motorA = 5, motorB = 6;

const int encoderLeft = 8, encoderRight = 9;

const int triggerPinFront = 12, echoPinFront = 13;
const int triggerPinLeft = 10, echoPinLeft = 11;

const int MIN_SPEED = 70;

// PID algorithm
const float SET_POINT = 15;
const float p = 0.5;

// Ultrasonic sensor
long duration;
const int MAX_DISTANCE = 25;
const int MIN_DISTANCE = 5;

#line 22 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
void setup();
#line 34 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
void loop();
#line 39 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
void followWall();
#line 52 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
float triggerSensor(int triggerPin, int echoPin);
#line 69 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
int pid(float distance);
#line 76 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
void forward(int leftSpeed, int rightSpeed);
#line 22 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v3\\lab_09_wall_following_v3.ino"
void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  Serial.begin(9600);
}

void loop() {
    followWall();
    delay(30);
}

void followWall() {
    float distance = triggerSensor(triggerPinLeft, echoPinLeft);
    float output = pid(distance);

    int leftSpeed = MIN_SPEED - output;
    int rightSpeed = MIN_SPEED + output;

    Serial.print("Output: ");
    Serial.print(output);

    forward(leftSpeed, rightSpeed);
}

float triggerSensor(int triggerPin, int echoPin) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH);

    float distance = (duration / 2) / 29.1;  // Divide by 29.1 or multiply by 0.0343

    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
    if (distance < MIN_DISTANCE) distance = MIN_DISTANCE;

    return distance;
}

int pid(float distance) {
    int error = distance - SET_POINT;
    int output = p * error;

    return output;
}

void forward(int leftSpeed, int rightSpeed) {
    analogWrite(motorA, leftSpeed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, rightSpeed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    Serial.print(" Left speed: ");
    Serial.print(leftSpeed);
    Serial.print(" Right speed: ");
    Serial.println(rightSpeed);
}
