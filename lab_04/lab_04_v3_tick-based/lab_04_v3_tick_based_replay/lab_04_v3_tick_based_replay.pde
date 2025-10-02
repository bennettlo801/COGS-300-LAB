import processing.serial.*;

Serial port;
String[] lines;
int index = 1; // skip header

boolean running = false;
boolean inStopBuffer = false;
int stopStartTime = 0;

char currentCommand = 'x';
long targetTicksRight = 0;
long targetTicksLeft  = 0;

// Tuning factors
float turnFactor = 1;    // 95% of recorded ticks for turns
float forwardFactor = 1; // 98% of recorded ticks for forward

// Tolerance for encoder comparison
long tolerance = 2;

// Stop command duration (ms) for gaps between moves
int stopDuration = 0;

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0]; // adjust if needed
  port = new Serial(this, portName, 115200);

  // Load CSV file
  lines = loadStrings("C:/Users/Justin/Downloads/lab_04_tick_based_record/commands_ticks_20251002_104616.csv"); // replace with your filename
  if (lines.length <= 1) {
    println("No commands in file!");
    exit();
  }

  startNextCommand();
}

void draw() {
  if (!running) return;

  // Read Arduino feedback
  while (port.available() > 0) {
    String line = port.readStringUntil('\n');
    if (line != null) {
      line = line.trim();
      // Expecting lines like: "w,5,7"
      String[] parts = split(line, ',');
      if (parts.length == 3) {
        char cmd = parts[0].charAt(0);
        long ticksR = Long.parseLong(parts[1].trim());
        long ticksL = Long.parseLong(parts[2].trim());

        boolean reachedTarget = false;

        // Turn-aware logic
        if (currentCommand == 'w') {
          // Forward: check both wheels
          reachedTarget = ticksR >= targetTicksRight - tolerance &&
                          ticksL >= targetTicksLeft - tolerance;
        } else if (currentCommand == 'q') {
          // Left turn: check only right wheel
          reachedTarget = ticksR >= targetTicksRight - tolerance;
        } else if (currentCommand == 'e') {
          // Right turn: check only left wheel
          reachedTarget = ticksL >= targetTicksLeft - tolerance;
        }

        if (reachedTarget) {
          port.write('x'); // stop
          inStopBuffer = true;
          stopStartTime = millis();
        }
      }
    }
  }

  // Handle stop buffer (gaps or stop command duration)
  if (inStopBuffer && millis() - stopStartTime >= stopDuration) {
    inStopBuffer = false;
    index++;
    if (index >= lines.length) {
      println("Replay finished");
      port.write('x');
      running = false;
      noLoop();
      return;
    }
    startNextCommand();
  }
}

void startNextCommand() {
  String[] parts = split(lines[index], ',');
  currentCommand = parts[0].charAt(0);

  if (currentCommand == 'x') {
    // Stop command: use duration
    stopDuration = int(parts[3]); // duration_ms from CSV
    port.write('x');
    println("Stop for " + stopDuration + " ms");
    inStopBuffer = true;
    stopStartTime = millis();
    running = true;
    return;
  }

  // Active movement command: get target ticks
  long rawRight = Long.parseLong(parts[1].trim());
  long rawLeft  = Long.parseLong(parts[2].trim());

  // Apply tuning factors
  if (currentCommand == 'w') {
    targetTicksRight = (long)(rawRight * forwardFactor);
    targetTicksLeft  = (long)(rawLeft  * forwardFactor);
  } else if (currentCommand == 'q' || currentCommand == 'e') {
    targetTicksRight = (long)(rawRight * turnFactor);
    targetTicksLeft  = (long)(rawLeft  * turnFactor);
  }

  println("Sending " + currentCommand + " to reach R:" + targetTicksRight + " L:" + targetTicksLeft);
  port.write(currentCommand);

  // Reset stopDuration for next gap
  stopDuration = 50; // default gap if next command is active
  running = true;
}
