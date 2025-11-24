#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
#include <Servo.h>

Servo myservo;

// Motor constants
const int motorApin1 = 7, motorApin2 = 4; // left motor
const int motorBpin1 = 3, motorBpin2 = 2; // right motor
const int motorA = 5, motorB = 6;

const int encoderLeft = 8, encoderRight = 9;

const int triggerPinFront = 12, echoPinFront = 13;
const int triggerPinLeft = 10, echoPinLeft = 11;

const int MIN_SPEED = 70;

// PID algorithm 
const float SET_POINT = 15.0; 
const float Kp = 0.15;
const float Ki = 0.0;
const float Kd = 0.8; 
float lastError = 0; // Store the error from the previous loop iteration
const float DT = 0.030; // Time step in seconds (30ms / 1000)

// Ultrasonic sensor
long duration;
const int MAX_DISTANCE = 25;
const int MIN_DISTANCE = 3;

// Servo
const int servoPin = 9;
int servoPosition = 90;
int incrementServo = 5;
unsigned long lastUpdate = 0;
const int SWEEP_ANGLE = 90;
const int SWEEP_DELAY_MS = 100;


#line 39 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void setup();
#line 54 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void loop();
#line 59 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void followWall();
#line 74 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void sweepFrontSensor();
#line 93 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
float triggerSensor(int triggerPin, int echoPin);
#line 117 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
int pid(float distance);
#line 132 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void forward(int leftSpeed, int rightSpeed);
#line 147 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void backward(int leftSpeed, int rightSpeed);
#line 39 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_09\\lab_09_wall_following_v4\\lab_09_wall_following_v4.ino"
void setup() {
  pinMode(motorApin1, OUTPUT), pinMode(motorApin2, OUTPUT);
  pinMode(motorBpin1, OUTPUT), pinMode(motorBpin2, OUTPUT);

  pinMode(motorA, OUTPUT), pinMode(motorB, OUTPUT);

  pinMode(triggerPinFront, OUTPUT), pinMode(echoPinFront, INPUT);
  pinMode(triggerPinLeft, OUTPUT), pinMode(echoPinLeft, INPUT);

  myservo.attach(servoPin);
  myservo.write(90);

  Serial.begin(9600);
}

void loop() {
    followWall();
    // sweepFrontSensor();
}

void followWall() {
    float distance = triggerSensor(triggerPinLeft, echoPinLeft);
    float output = pid(distance);

    int leftSpeed = MIN_SPEED - output;
    int rightSpeed = MIN_SPEED + output;

    forward(leftSpeed, rightSpeed);

    if (triggerSensor(triggerPinFront, echoPinFront) < MIN_DISTANCE + 2) {
        backward(MIN_SPEED, MIN_SPEED);
        delay(300);
    }
}

void sweepFrontSensor() {
  unsigned long currentTime = millis();

  // Check if enough time has passed to perform the next step
  if (currentTime - lastUpdate >= SWEEP_DELAY_MS) {

    servoPosition += incrementServo;
    myservo.write(servoPosition);

    if (servoPosition >= (90 + (SWEEP_ANGLE / 2)) || servoPosition <= (90 - (SWEEP_ANGLE / 2))) {
        incrementServo = -incrementServo;
    }

    lastUpdate = currentTime; 
  }

  Serial.println("Servo: " + String(servoPosition));
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

    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;

    return distance;
}

int pid(float distance) {
    float error = distance - SET_POINT;

    float P_out = Kp * error;

    float derivative = (error - lastError) / DT; 
    float D_out = Kd * derivative;

    lastError = error; 

    float output_float = P_out + D_out;

    return (int)output_float;
}

void forward(int leftSpeed, int rightSpeed) {
    analogWrite(motorA, leftSpeed);
    digitalWrite(motorApin1, HIGH);
    digitalWrite(motorApin2, LOW);

    analogWrite(motorB, rightSpeed);
    digitalWrite(motorBpin1, HIGH);
    digitalWrite(motorBpin2, LOW);

    // Serial.print(" Left speed: ");
    // Serial.print(leftSpeed);
    // Serial.print(" Right speed: ");
    // Serial.println(rightSpeed);
}

void backward(int leftSpeed, int rightSpeed) {
    analogWrite(motorA, leftSpeed * 2);
    digitalWrite(motorApin1, LOW);
    digitalWrite(motorApin2, HIGH);

    analogWrite(motorB, rightSpeed * 3);
    digitalWrite(motorBpin1, LOW);
    digitalWrite(motorBpin2, HIGH);
}
