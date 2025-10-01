import processing.serial.*;

Serial port;
String[] lines;
int index = 1; // skip header

char currentCommand = 'x';
int currentDuration = 0;
int startTime = 0;

boolean running = false;
boolean inStopBuffer = false;
int stopBufferDuration = 100; // ms pause between commands

// Factor to shorten turns (tune experimentally)
float turnFactor = 0.95;  // 95% of recorded duration for turning

// float driveFactor = 0.98; // 98% of recorded duration for driving forward (if needed) *****

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0]; // change if needed
  port = new Serial(this, portName, 115200);

  // Load the CSV
  lines = loadStrings("C:/Users/Justin/Downloads/lab4record2_0/commands_20250930_175454.csv"); // replace filename
  if (lines.length <= 1) {
    println("No commands in file!");
    exit();
  }

  startNextCommand();
}

void draw() {
  if (!running) return;

  int elapsed = millis() - startTime;

  if (!inStopBuffer && elapsed >= currentDuration) {
    // End of a command → stop first
    port.write('x');
    println("Stop buffer");
    inStopBuffer = true;
    startTime = millis();
  } 
  else if (inStopBuffer && elapsed >= stopBufferDuration) {
    // After stop buffer → next command
    index++;
    if (index >= lines.length) {
      println("Replay finished");
      port.write('x'); // final stop
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
  currentDuration = int(parts[1]);

  // If this is a turn, shorten the replay duration
  if (currentCommand == 'q' || currentCommand == 'e') {
    currentDuration = int(currentDuration * turnFactor);
  }
  
 // If this is driving forward, shorten the replay duration *****
 // if (currentCommand == 'w') {
 //   currentDuration = int(currentDuration * driveFactor);
 // }

  println("Sending " + currentCommand + " for " + currentDuration + " ms");
  port.write(currentCommand);

  startTime = millis();
  inStopBuffer = false;
  running = true;
}
