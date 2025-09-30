import processing.serial.*;
import java.io.BufferedReader;
import java.io.FileReader;

Serial port;
String[] lines;
int index = 1; // start after header

float prevRight = 0;
float prevLeft  = 0;

void setup() {
  // Open Arduino serial port
  println(Serial.list());
  String portName = Serial.list()[0]; // change if needed
  port = new Serial(this, portName, 115200);

  // Load CSV
  String filename = "lab_04_replay/arduino_log_20250930_124619.csv"; // replace with your file
  try {
    BufferedReader reader = new BufferedReader(new FileReader(filename));
    lines = reader.lines().toArray(String[]::new);
    reader.close();
  } catch (Exception e) {
    println("Error reading CSV: " + e);
    exit();
  }

  frameRate(60); // controls playback speed
}

void draw() {
  if (index >= lines.length) {
    println("Playback finished");
    noLoop();
    return;
  }

  // parse current line
  String line = lines[index];
  if (line.trim().length() == 0 || line.startsWith("t_ms")) {
    index++;
    return;
  }

  String[] parts = split(line, ',');
  float currRight = float(parts[1]);
  float currLeft  = float(parts[2]);

  // determine movement
  String command = "";
  if (currRight > prevRight && currLeft > prevLeft) {
    command = "w"; // forward
  } else if (currRight < prevRight && currLeft < prevLeft) {
    command = "s"; // backward
  } else if (currRight > prevRight && currLeft < prevLeft) {
    command = "q"; // rotate left
  } else if (currRight < prevRight && currLeft > prevLeft) {
    command = "e"; // rotate right
  } else if (currRight > prevRight) {
    command = "d"; // right
  } else if (currLeft > prevLeft) {
    command = "a"; // left
  } else {
    command = "x"; // stop
  }

  // send command to Arduino
  port.write(command);

  // store current values for next frame
  prevRight = currRight;
  prevLeft  = currLeft;

  index++;
}
