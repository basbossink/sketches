/**
 * Simple Write. 
 * 
 * Check if the mouse is over a rectangle and writes the status to the serial port. 
 * This example works with the Wiring / Arduino program that follows below.
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
int servo1Pos = 0;
int servo2Pos = 1;
PFont font;
void setup() 
{
  size(500, 200);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  font = createFont("Tahoma", 24);
  textFont(font);
  fill(0);
}

void draw() {
  background(255);
  textAlign(LEFT);
  text("Servo 1:", width * 0.1, 50);
  text("Servo 2:", width * 0.70, 50);
  text("123", width * 0.15, 80);
  text("123", width * 0.75, 80);
    
    //myPort.read();              // send an H to indicate mouse is over square
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      //myPort.write('U');
    } else if (keyCode == DOWN) {
      //myPort.write('D');
    } else if (keyCode == RIGHT) {
      //myPort.write('R');
    } else if (keyCode == LEFT) {
      //myPort.write('L');
    }
  } 
}
/*
  // Wiring/Arduino code:
 // Read data from the serial and turn ON or OFF a light depending on the value
 
 char val; // Data received from the serial port
 int ledPin = 4; // Set the pin to digital I/O 4
 
 void setup() {
 pinMode(ledPin, OUTPUT); // Set pin as OUTPUT
 Serial.begin(9600); // Start serial communication at 9600 bps
 }
 
 void loop() {
 if (Serial.available()) { // If data is available to read,
 val = Serial.read(); // read it and store it in val
 }
 if (val == 'H') { // If H was received
 digitalWrite(ledPin, HIGH); // turn the LED on
 } else {
 digitalWrite(ledPin, LOW); // Otherwise turn it OFF
 }
 delay(100); // Wait 100 milliseconds for next reading
 }
 
 */
