import processing.serial.*;
import java.io.PrintWriter;

Serial port;
PrintWriter writer;

char currentCommand = 'x'; // last sent command
long lastTime = 0;

String serialBuffer = "";
long lastStopTime = 0;

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0]; // adjust if needed
  port = new Serial(this, portName, 115200);

  String filename = "commands_ticks_" + year() + nf(month(),2) + nf(day(),2) + "_" + nf(hour(),2) + nf(minute(),2) + nf(second(),2) + ".csv";
  writer = createWriter(filename);
  writer.println("command,deltaTicksRight,deltaTicksLeft,duration_ms");

  lastTime = millis();
  lastStopTime = lastTime;
}

void draw() {
  // 1️⃣ Read serial from Arduino
  while (port.available() > 0) {
    char c = port.readChar();
    if (c == '\n' || c == '\r') {
      parseSerialLine(serialBuffer.trim());
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // 2️⃣ Check keyboard input
  char newCommand = 'x';
  if (keyPressed) {
    if (key == 'w') newCommand = 'w';
    else if (key == 'q') newCommand = 'q';
    else if (key == 'e') newCommand = 'e';
  }

  long now = millis();

  // 3️⃣ Handle command change
  if (newCommand != currentCommand) {
    // If last command was a stop, record duration
    if (currentCommand == 'x') {
      long stopDuration = now - lastStopTime;
      if (stopDuration > 0) {
        writer.println("x,0,0," + stopDuration);
        writer.flush();
      }
    }

    // Send new command
    port.write(newCommand);
    currentCommand = newCommand;

    // Record the time for stop gaps
    if (currentCommand == 'x') {
      lastStopTime = now;
    }
  }
}

// Parse Arduino tick messages
void parseSerialLine(String line) {
  // Expecting lines like: "REC,w,5,7"
  if (line.startsWith("REC")) {
    String[] parts = split(line, ',');
    if (parts.length == 4) {
      long now = millis();
      int duration = int(now - lastStopTime); // duration since last stop or command
      writer.println(parts[1] + "," + parts[2] + "," + parts[3] + ",0"); // movement duration will be handled by ticks
      writer.flush();

      // Reset stop timer
      lastStopTime = now;
    }
  }
}

void stop() {
  // log last stop duration before exit
  long stopDuration = millis() - lastStopTime;
  if (stopDuration > 0) {
    writer.println("x,0,0," + stopDuration);
    writer.flush();
  }

  port.write('x');
  writer.close();
  super.stop();
}
