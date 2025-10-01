# COGS 300 Robot Project

Repository for our autonomous robot that will compete in an obstacle maze. Functionality is organized by lab.

## Team InCOGnito

Aaron, Bennett, Justin, Yalda

## Labs

### Lab 03

Using the serial monitor, you can enter:
 - 'w' to drive the robot forwards,
 - 's' to drive the robot backwards,
 - 'a' to turn the robot left while driving,
 - 'd' to turn the robot right while driving,
 - 'q' to rotate the robot left while in place,
 - 'e' to rotate the robot right while in place,
 - any other button to stop the robot.

Using the Processing sketch, use the same controls, except the robot will stop when there is no input.

### Lab 04

After uploading lab_04.ino, use the record.pde processing sketch to control the robot and save the movement to a .csv file

Controls:
 - 'w' to drive the robot forwards,
 - 'a' to pivot the robot left,
 - 'd' to pivot the robot right,
 - 's' to stop all movement.

 Add the created arduino_log file to the replay.pde sketch folder, and run the sketch to playback the movement.
 The arduino log included in the replay folder moves the robot forward, then makes a 90 degree turn left.