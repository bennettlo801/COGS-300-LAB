int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

void setup()
{
    pinMode(motor1pin1, OUTPUT);
    pinMode(motor1pin2, OUTPUT);
    pinMode(motor2pin1, OUTPUT);
    pinMode(motor2pin2, OUTPUT);

    pinMode(9, OUTPUT);  // ENA
    pinMode(10, OUTPUT); // ENB

    Serial.begin(9600); // Start serial communication
}

void loop()
{
    analogWrite(9, 100);  // ENA pin
    analogWrite(10, 100); // ENB pin

    if (Serial.available() > 0)
    {
        // Read the input as a string
        String input = Serial.readStringUntil('\n');
        input.trim(); // remove \r or extra spaces

        if (input == "forward")
        {
            forward();
        }
        else if (input == "stop")
        {
            backward();
        }
        else
        {
            stop();
        }
    }
}

void forward()
{
    digitalWrite(motor1pin1, HIGH);
    digitalWrite(motor1pin2, LOW);

    digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
}

void backward()
{
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, HIGH);

    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, HIGH);
}

void stop()
{
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
}
