// Andrew W. Carvey    9.16.2014
// Arduino             Cuttlefish Testing: Causal Movement

/*
  This program controls the motion of a feeding skewer for a cuttlefish in order to assess intelligence.
 A continuous rotational hobby "servo" is connected to a Pololu microMaestro control module.
 The feeding skewer is attached to the servo.
 A green LED is used to visually indicated the start and end of each trial.
 Four tests are conducted to assess intelligence:
 1) Foraging
 2) Patterned Movement
 3) Casual Movement
 4) Reactive Movement
 Each test is composed of four variations. Each variation is conducted Monday - Thursday.
 A description of the tests and variations are described in detail in each function.
 
 The arduino (MCU) interfaces with a host computure running custom software named "Bonsai".
 The MCU and Bonsai communicate via asynchronous data transfer via the serial port.
 Bonsai is the main user interface which passes both function and state commands to the arduino.
 
 */


/***********************************************/
/*                                             */
/*  Program Initialisation                     */
/*                                             */
/***********************************************/

#include <SoftwareSerial.h>
SoftwareSerial maestroSerial(8, 9);  // Digital pins 8 & 9 are used for software serial RX & TX

#define servoStop 6000

int pulseHigh, pulseLow;

int StopButton = 2;
int GoButton = 4;

int bonsaiGoPin = 3;
int bonsaiStopPin = 6;

boolean GoButtonVal, GoButtonPrev;
volatile boolean StopButtonVal, StopButtonPrev;
boolean bonsaiGoState=0;
boolean bonsaiStopState=0;
boolean direction, movement;

volatile boolean testStarted=0;

int cueLED = 5;
boolean cueLEDstate=0;

int tMotorOn;

unsigned long randomWait, randomWait0, randomWait1, randomWait2;
int numWiggles;

int i, j, n;

unsigned long currentTime;
unsigned long tButtonPoll;
unsigned long tRandomWait;
unsigned long tMovementStart;
unsigned long tMovementComplete;
unsigned long tMovementStartRandom;
volatile unsigned long tStop;
volatile unsigned long tStopPrev;


/***********************************************/
/*                                             */
/*  Program Setup                              */
/*                                             */
/***********************************************/
void setup() {
  // Start serial connection
  Serial.begin(19200);
  maestroSerial.begin(9600);
  delay(200);

  maestroSerial.write(0xAA);     delay(200);
  stopServoPulses();

  // sets up I/O of pins
  pinMode(StopButton, INPUT_PULLUP);
  pinMode(GoButton, INPUT_PULLUP);
  pinMode(bonsaiGoPin, INPUT_PULLUP);  
  pinMode(bonsaiStopPin, INPUT_PULLUP);  
  pinMode(cueLED, OUTPUT);
  digitalWrite(cueLED, LOW);
  
  /*** Servo Velocity Calibration Offsets. Change the values inside the (nnn) ***/
//  pulseHigh = 6000+ (850);      //counter clockwise @ 1Hz
//  pulseLow =  6000- (820);      //clockwise @ 1Hz

//  pulseHigh = 6000+ (362);      //counter clockwise @ 1/2Hz
//  pulseLow =  6000- (365);      //clockwise @ 1/2Hz
  
  pulseHigh = 6000+ (170);      //counter clockwise @ 1/5Hz
  pulseLow =  6000- (250);      //clockwise @ 1/5Hz

//  pulseHigh = 6000+ (090);      //counter clockwise @ 1/8Hz
//  pulseLow =  6000- (115);      //clockwise @ 1/8Hz

//  pulseHigh = 6000+ (56);      //counter clockwise @ 1/10Hz
//  pulseLow =  6000- (112);      //clockwise @ 1/10Hz


  // use noise from A0 to seed random number generator
  randomSeed(analogRead(A0));

  // Prints to the Serial Monitor window: 
  // Start Serial Communication. 
  while (! Serial); // Wait untilSerial is ready
  Serial.println(" ");
  Serial.println("__START SERIAL SIGNAL__ "); 
  Serial.println("Please position arm vertically at 'Noon' position."); 
  Serial.println("Press Star button to start Causal Movement test.  RED button stops experiment.");   
  Serial.println(" ");
}



/***********************************************/
/*                                             */
/*  stopExperiment                             */
/*                                             */
/***********************************************/
void stopExperiment() {
  Serial.println("--Trial stopped by experimenter, catch or reset--");   
  testStarted=0;
  digitalWrite(cueLED, LOW);  //Turn of LED
  stopServoPulses();
  Serial.println("Trial Stopped.   Trial Stopped.   Trial Stopped.   ");
  Serial.println("  ");  
  delay(250);     //delay 0.25sec (allow time for finger to leave STOP button)
}


/***********************************************/
/*                                             */
/*  cw360                                      */
/*                                             */
/***********************************************/
void cw360() {
  // Rotate 360 Clockwise.
  moveServo(0, pulseLow); //Move the servo Clockwise

  tMovementStart=millis();
  //Change this value to affect how long the servo rotates
  tMotorOn=3500;     //experiment=10000.  calibration=10000, 20000 or 30000
  while(((tMovementStart + tMotorOn) > millis()) && digitalRead(2)) { }  //waits for 10sec
  if(digitalRead(2)==0) {
    Serial.println("Stop Button Pressed.");
    stopExperiment();
    delay(200);
    return;
  }
  stopServoPulses();
}


/***********************************************/
/*                                             */
/*  ccw360                                     */
/*                                             */
/***********************************************/
void ccw360() {
  // Rotate 360 Counter Clockwise.
  moveServo(0, pulseHigh);    // Move the servo Counter Clockwise

  tMovementStart=millis();
  //Change this value to affect how long the servo rotates 
  tMotorOn=3700;   //experiment=10000.  calibration=10000, 20000 or 30000
  while(((tMovementStart + tMotorOn) > millis()) && digitalRead(2)) { }  //waits for 10sec
  if(digitalRead(2)==0) {
    Serial.println("Stop Button Pressed.");
    stopExperiment();
    delay(200);
    return;
  }
  stopServoPulses();
}



/***********************************************/
/*                                             */
/*  cw180                                      */
/*                                             */
/***********************************************/
void cw180() {
  // Rotate 180 Clockwise.
  moveServo(0, pulseLow);   // Move the servo Clockwise

  tMovementStart=millis();
  //Change this value to affect how long the servo rotates 
  tMotorOn=1790;   //experiment=5000.
  while(((tMovementStart + tMotorOn) > millis()) && digitalRead(2)) { }  //waits for 5s
  if(digitalRead(2)==0) {
    Serial.println("Stop Button Pressed.");
    stopExperiment();
    return;
  }
  stopServoPulses();
}


/***********************************************/
/*                                             */
/*  ccw180                                     */
/*                                             */
/***********************************************/
void ccw180() {
  // Rotate 180 Counter Clockwise.
  moveServo(0, pulseHigh);     // Move the servo Counter Clockwise

  tMovementStart=millis();
  //Change this value to affect how long the servo rotates 
  tMotorOn=1900;   //experiment=5000.
  while(((tMovementStart + tMotorOn) > millis()) && digitalRead(2)) { }  //waits for 5s
  if(digitalRead(2)==0) {
    Serial.println("Stop Button Pressed.");
    stopExperiment();
    return;
  }
  stopServoPulses();
}

/***********************************************/
/*                                             */
/*  Pause                                      */
/*                                             */
/***********************************************/
void pause() {
  // Pause for duration during 180 rotation trial.
  tMovementStart=millis();

  //Change this value to affect how long the servo PAUSES
  int tPauseDuration=2000;   //experiment=[2000-5000].
  Serial.print("Pause for "); Serial.print(tPauseDuration/1000); Serial.println(" seconds");

  while(((tMovementStart + tPauseDuration) > millis()) && digitalRead(2)) { }  //waits for X_seconds
  if(digitalRead(2)==0) {
    stopExperiment();
    return;
  }
}




/***********************************************/
/*                                             */
/*  Program Loop                               */
/*                                             */
/***********************************************/
void loop() {
  // Start Causal Movement Test after button is pressed
  currentTime = millis();
  
  //Motion started when GO button pressed
  if(currentTime > tButtonPoll+10) {
    tButtonPoll=currentTime;
    GoButtonVal = digitalRead(GoButton);
    if(GoButtonVal==LOW && GoButtonPrev==HIGH) {  //Determine if button just pressed.
      testStarted=1;  // Notes that the test has been manually started.
      direction = 0;  //Starts Clockwise motion
      causalMovement();
    }
    GoButtonPrev = GoButtonVal; 
  }
  
  //Next Trial started after random interval
  if(testStarted && (currentTime >= tMovementStartRandom)) {
    causalMovement();
  }  

  //Allows trial to be stopped during wait interval.
  if(digitalRead(2)==0) {   
    stopExperiment();
    delay(2000);
  }    
}


/***********************************************/
/*                                             */
/*  Causal Movement                            */
/*                                             */
/***********************************************/
void causalMovement() {
  Serial.println("-- CAUSAL MOVEMENT TEST ACTIVATED-- ");
  digitalWrite(cueLED, HIGH);
  delay(2000);

  if(direction==0) {
    Serial.print("CW Trial:  ");
    movement = analogRead(A0) % 2;  //generate "random" even or odd number    
    if(movement==0) {
      Serial.println("180 Rotation");      
      cw180();
      if(testStarted==1)  pause();   //Verify Trial was not stopped by experimenter
      if(testStarted==1)  ccw180();  //Verify Trial was not stopped by experimenter
    }
    if(movement==1) {
      Serial.println("360 Rotation");    
      cw360();    
    }
  }
  
  if(direction==1) {
    Serial.print("CCW Trial:  ");
    movement = analogRead(A0) % 2;  //generate "random" even or odd number
    if(movement==0) {
      Serial.println("180 Rotation");       
      ccw180();
      if(testStarted==1)  pause();  //Verify Trial was not stopped by experimenter
      if(testStarted==1)  cw180();  //Verify Trial was not stopped by experimenter
    }
    if(movement==1) {
      Serial.println("360 Rotation");   
      ccw360();
    }
  }

  //toggles the start direction
  direction=!direction;      
 
  if(testStarted==1 && digitalRead(2)) {
    digitalWrite(cueLED, LOW);  
    Serial.println("--End of Causal Movement Round--");      
    tMovementComplete=millis();
    randomWait0=random(10, 15);  //Change these values to increase wait time.
    randomWait1=random(10, 20);
    randomWait2=random(10, 25);
    randomWait = (randomWait0 + randomWait1 + randomWait2)/3;
    tMovementStartRandom = tMovementComplete + randomWait*1000; 
    Serial.print("  Next test begins in "); Serial.print(randomWait);
    Serial.println(" seconds.");  Serial.println("  ");
  }
}


/***********************************************/
/*                                             */
/*  Move Servo Function                        */
/*                                             */
/***********************************************/
void moveServo(int ServoChannel, int target) {
  //1000us PWM pulse represents a servo angle of 0 degrees.
  //2000us PWM pulse represents a servo angele of 90 degrees.
  //These values could vary based on the servo you use, check your servo's 
  //spec documentation and verify what PWM pulses are needed to move it.

  byte serialBytes[4]; //Create the byte array object that will hold the communication packet.
  Serial.print("  Sending Servo pulse length = ");
  Serial.println(target);
  serialBytes[0] = 0x84; // Command byte: Set Target.
  serialBytes[1] = ServoChannel; // First byte holds channel number.
  serialBytes[2] = target & 0x7F; // Second byte holds the lower 7 bits of target.
  serialBytes[3] = (target >> 7) & 0x7F; // Third byte holds the bits 7-13 of target.
  maestroSerial.write(serialBytes, sizeof(serialBytes)); //Write the byte array to the serial port.
}


/***********************************************/
/*                                             */
/*  Velocity Servo Function                    */
/*                                             */
/***********************************************/
void velocityServo(int ServoChannel, int velocity) {
  //This function limits the Max speed at which a servo channel's output value changes.
  //Velocity limit is a value from 0 - 400 in units of (0.25 us)/(10 ms).
  //A velocity of 400 moves a servo from a target of 1ms - 2ms in 0.1s  (90degress/0.1s)
  //A velocity of 140 corresponds to 3.5 us/ms.  Adjusts target from 1000us to 1350us in 0.1s.
  //A velocity of 0 corresponds to no velocity limit.
  //At the Min speed of 1, the servo takes 40 seconds to move from 1ms - 2ms.

  byte serialBytes[4]; //Create the byte array object that will hold the communication packet.
  serialBytes[0] = 0x87; // Command byte: Set Target.
  serialBytes[1] = ServoChannel; // First byte holds channel number.
  serialBytes[2] = velocity & 0x7F; // Second byte holds the lower 7 bits of target.
  serialBytes[3] = (velocity >> 7) & 0x7F; // Third byte holds the bits 7-13 of target.
  maestroSerial.write(serialBytes, sizeof(serialBytes)); //Write the byte array to the serial port.
}


/***********************************************/
/*                                             */
/*  Stop Servo Pulses                          */
/*                                             */
/***********************************************/
void stopServoPulses() {
  Serial.println("  **Servo Pulses Stopped - Free to rotate Servo manually.");  
  byte serialBytes[4]; //Create the byte array object that will hold the communication packet.
  serialBytes[0] = 0x84;           // Command byte: Set Target.
  serialBytes[1] = 0x00;           // Hard Code Servo_Channel_0
  serialBytes[2] = 0x00;           // Second byte holds the lower 7 bits of target.
  serialBytes[3] = 0x00;           // Third byte holds the bits 7-13 of target.
  maestroSerial.write(serialBytes, sizeof(serialBytes)); //Write the byte array to the serial port.
}



/***********************************************/
/*                                             */
/*  Servo Motion Functioning Verification      */
/*                                             */
/***********************************************/
void servoVerification() {
  // This function runs through four servo motion tests to verify functionality
  // Each test is conducted in both Left and Right mirrored behavior
  Serial.println("  ");
  Serial.println("---------------------------------------------------------  ");
  Serial.println("Starting Servo Motion Ability Test");
  Serial.println("  ");

  velocityServo(0, 0);   // Servo_0  Unlimited "velocity" (acceleration with continuous rotational servo)


  /***** Servo "Wiggle" *****/
  Serial.println("  ");
  Serial.println("Starting Servo Wiggle Function");
  servoWiggle();

  /***** 360 degree rotation *****/
  Serial.println("  ");
  Serial.println("Starting 360 Rotation Function");  
  ccw360();            // Rotate Counter Clockwise 360 degrees
  delay(2000);         // Pause prior next function test
  cw360();             // Rotate Clockwise 360 degrees

  /***** 180 degree rotation (L/R), PAUSE,  180 degree rotation (L/R) *****/
  Serial.println("  ");
  Serial.println("Starting 180+Pause+180 Function");  
  ccw180();            // Rotate Counter Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  ccw180();            // Rotate Counter Clockwise 180 degrees
  delay(2000);         // Assumuming rotates at 1Hz

  cw180();             // Rotate Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  cw180();             // Rotate Clockwise 180 degrees

  /*** 180 degree rotation (L/R), PAUSE,  180 degree rotation (R/L) ***/
  Serial.println("  ");
  Serial.println("Starting 180+Pause+!180 Function"); 
  Serial.println("  ");
  ccw180();            // Rotate Counter Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  cw180();            // Rotate Counter Clockwise 180 degrees
  delay(2000);         // Assumuming rotates at 1Hz

  cw180();             // Rotate Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  ccw180();             // Rotate Clockwise 180 degrees

  Serial.println("  ");
  Serial.println("End of Servo Motion Ability Test."); 
  Serial.println("---------------------------------------------------------  ");
  Serial.println("  ");

}




/***********************************************/
/*                                             */
/*  Servo Wiggle                               */
/*                                             */
/***********************************************/
void servoWiggle() {
  for(i=0; i<2; i++) {
    moveServo(0, 465); //dance left
    delay(125);
    moveServo(0, servoStop);  //Stops servo rotation

    moveServo(0, 552); //dance right    
    delay(250);
    moveServo(0, servoStop);  //Stops servo rotation
  
    moveServo(0, 465); //dance left
    delay(125);
    moveServo(0, servoStop);  //Stops servo rotation    
  }
  moveServo(0, servoStop); //return to neutral
}


