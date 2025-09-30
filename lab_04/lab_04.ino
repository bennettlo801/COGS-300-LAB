#include <cmath>

const int encoderRight = 6, encoderLeft = 7;

// encoder specs
const int ticks_per_rev = 40;
const float wheel_radius = 65.0 / 2; // mm

// constants
const float distance_per_rev  = 2.0 * M_PI * wheel_radius; 
const float distance_per_tick = distance_per_rev / ticks_per_rev;

long ticksRight = 0;
long ticksLeft  = 0;

int lastRight = LOW;
int lastLeft  = LOW;

void setup() {
  pinMode(encoderRight, INPUT_PULLUP);
  pinMode(encoderLeft, INPUT_PULLUP);
  Serial.begin(115200);

  lastRight = digitalRead(encoderRight);
  lastLeft  = digitalRead(encoderLeft);
}

void loop() {
  int v1 = digitalRead(encoderRight);
  int v2 = digitalRead(encoderLeft);

  // Detect edge on right wheel
  if (v1 != lastRight) {
    ticksRight++;
    lastRight = v1;
  }

  // Detect edge on left wheel
  if (v2 != lastLeft) {
    ticksLeft++;
    lastLeft = v2;
  }

  // compute distance so far
  float distanceRight = ticksRight * distance_per_tick;
  float distanceLeft  = ticksLeft  * distance_per_tick;

  Serial.print(distanceRight); Serial.print(",");
  Serial.println(distanceLeft);


  // Serial.print("Right: ");
  // Serial.print(ticksRight);
  // Serial.print(" ticks, ");
  // Serial.print(distanceRight);
  // Serial.println(" mm");

  // Serial.print("Left: ");
  // Serial.print(ticksLeft);
  // Serial.print(" ticks, ");
  // Serial.print(distanceLeft);
  // Serial.println(" mm");

  delay(10);
}
