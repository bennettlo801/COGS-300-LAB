import processing.serial.*;
import java.io.PrintWriter;

Serial port;
PrintWriter writer;

char currentCommand = 'x';
long lastTime = 0;

void setup() {
  println(Serial.list());
  String portName = Serial.list()[0]; // change if needed
  port = new Serial(this, portName, 115200);

  String filename = "commands_" + year() + nf(month(),2) + nf(day(),2) + "_" + nf(hour(),2) + nf(minute(),2) + nf(second(),2) + ".csv";
  writer = createWriter(filename);
  writer.println("command,duration_ms");

  lastTime = System.nanoTime();
}

void draw() {
  char newCommand = 'x';

  if (keyPressed) {
    if (key == 'w') newCommand = 'w';
    else if (key == 's') newCommand = 's';
    else if (key == 'q') newCommand = 'q';
    else if (key == 'e') newCommand = 'e';
  }

  if (newCommand != currentCommand) {
    long now = System.nanoTime();
    int duration = int((now - lastTime) / 1_000_000); // ms

    if (duration > 0) {
      writer.println(currentCommand + "," + duration);
      writer.flush();
    }

    currentCommand = newCommand;
    port.write(currentCommand);
    lastTime = now;
  }
}

void stop() {
  // log last segment before exit
  long now = System.nanoTime();
  int duration = int((now - lastTime) / 1_000_000);
  if (duration > 0) {
    writer.println(currentCommand + "," + duration);
    writer.flush();
  }

  writer.close();
  super.stop();
}
