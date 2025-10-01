#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
#include <cmath>

// motor Constants
const int SPEED = 175;

int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

int motorRight = 9;
int motorLeft = 10;

// encoder constants
const int encoderRight = 6, encoderLeft = 7;

// encoder specs
const int ticks_per_rev = 40;
const int wheel_diameter = 65; // mm

const int distance_per_rev = M_PI * wheel_diameter;
const int distance_per_tick = distance_per_rev / ticks_per_rev;

int ticksRight = 0;
int ticksLeft = 0;

int lastRight = LOW;
int lastLeft = LOW;

#line 31 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void setup();
#line 51 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void loop();
#line 57 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void motorInput();
#line 84 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void forward();
#line 92 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void right();
#line 103 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void left();
#line 114 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void stop();
#line 124 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void encode();
#line 31 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_04\\lab_04.ino"
void setup()
{
	// motor
	pinMode(motor1pin1, OUTPUT);
	pinMode(motor1pin2, OUTPUT);
	pinMode(motor2pin1, OUTPUT);
	pinMode(motor2pin2, OUTPUT);

	pinMode(motorRight, OUTPUT);
	pinMode(motorLeft, OUTPUT);

	// encoder
	pinMode(encoderRight, INPUT_PULLUP);
	pinMode(encoderLeft, INPUT_PULLUP);
	Serial.begin(115200);

	lastRight = digitalRead(encoderRight);
	lastLeft = digitalRead(encoderLeft);
}

void loop()
{
	motorInput();
	encode();
}

void motorInput()
{
	if (Serial.available() > 0)
	{
		char input = Serial.read();

		switch (input)
		{
		case 'w':
			forward();
			break;
		case 's':
			stop();
			break;
		case 'a':
			left();
			break;
		case 'd':
			right();
			break;
		default:
			stop();
			break;
		}
	}
}

void forward()
{
	analogWrite(motorRight, SPEED);
	analogWrite(motorLeft, SPEED);
	digitalWrite(motor1pin1, HIGH);
	digitalWrite(motor2pin1, HIGH);
}

void right()
{
	analogWrite(motorRight, 0);
	digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    
	analogWrite(motorLeft, SPEED);
    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void left()
{
	analogWrite(motorLeft, 0);
	digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
    
	analogWrite(motorRight, SPEED);
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);
}

void stop()
{
	analogWrite(motorRight, 0);
	analogWrite(motorLeft, 0);
	digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
}

void encode()
{
	int v1 = digitalRead(encoderRight);
	int v2 = digitalRead(encoderLeft);

	// Detect edge on right wheel
	if (v1 != lastRight)
	{
		ticksRight++;
		lastRight = v1;
	}

	// Detect edge on left wheel
	if (v2 != lastLeft)
	{
		ticksLeft++;
		lastLeft = v2;
	}

	// compute distance so far
	int distanceRight = ticksRight * distance_per_tick;
	int distanceLeft = ticksLeft * distance_per_tick;

	Serial.print(distanceRight);
	Serial.print(",");
	Serial.println(distanceLeft);

	delay(10);
}

