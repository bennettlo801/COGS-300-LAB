#include <Arduino.h>
#line 1 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

int enA = 9;    //right wheel
int enB = 10;   //left wheel

#line 10 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void setup();
#line 23 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void loop();
#line 63 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void forward();
#line 75 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void backward();
#line 87 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void left();
#line 99 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void right();
#line 111 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void rotateLeft();
#line 123 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void rotateRight();
#line 135 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void stop();
#line 10 "C:\\Users\\benne\\OneDrive - UBC\\2025W1\\COGS-300\\Lab\\lab_03\\lab_03.ino"
void setup()
{
    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(enA, OUTPUT); // ENA
    pinMode(enB, OUTPUT); // ENB

    Serial.begin(9600); // Start serial communication
}

void loop()
{
    if (Serial.available() > 0)
    {
        // Read the input as a char
        char input = Serial.read();
        Serial.print("input received: ");
        Serial.println(input);

        if (input == 'w')
        {
            forward();
        }
        else if (input == 's')
        {
            backward();
        }
        else if (input == 'a')
        {
            left();
        }
        else if (input == 'd')
        {
            right();
        }
        else if (input == 'q')
        {
            rotateLeft();
        }
        else if (input == 'e')
        {
            rotateRight();
        }
        else
        {
            stop();
        }
    }
}

void forward()
{
    analogWrite(enA, 150);
    analogWrite(enB, 150);

    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void backward()
{
    analogWrite(enA, 150);
    analogWrite(enB, 150);

    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);

    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
}

void left()
{
    analogWrite(enA, 200);
    analogWrite(enB, 125);

    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void right()
{
    analogWrite(enA, 125);
    analogWrite(enB, 200);

    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void rotateLeft()
{
    analogWrite(enA, 150);
    analogWrite(enB, 150);

    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
}

void rotateRight()
{
    analogWrite(enA, 150);
    analogWrite(enB, 150);

    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);

    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void stop()
{
    analogWrite(enA, 150);
    analogWrite(enB, 150);

    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
}

