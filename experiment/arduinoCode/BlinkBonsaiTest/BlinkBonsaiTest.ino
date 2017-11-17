#include <Boards.h>
#include <Firmata.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 5;
int Bonsai = 3;
int BonsaiPin = 1;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
  pinMode(Bonsai, INPUT_PULLUP); 
 
  Serial.begin(19200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200); 
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println("Ready.");
  digitalWrite(led, HIGH);
  delay(3000);
  digitalWrite(led, LOW);
  delay(3000);
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    if (inputString == "1") {              // wait for a second
      digitalWrite(led, HIGH);    // turn the LED off by making the voltage LOW
      delay(1500);
      digitalWrite(led, LOW);
      delay(1500);
      digitalWrite(led, HIGH);
      delay(1500);
      digitalWrite(led, LOW);
      delay(5000);
  }          
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
