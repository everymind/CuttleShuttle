// Andrew W. Carvey    9.7.2014
// Edited Danbee Kim   9.8.2014
// Arduino             Cuttlefish Testing

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
#include <Array.h>
SoftwareSerial maestroSerial(8, 9);  // Digital pins 8 & 9 are used for software serial RX & TX

#define pulseOffset 222

int pulseHigh, pulseLow;

int button0 = 2;
int bonsaiGoPin = 3;
int bonsaiStopPin = 4;


//const byte size = 100;
//int rawArray[size];
//Array<int> button0Prev = Array<int>(rawArray,size);

boolean button0Val, button0Prev, button0Prev2Sec;
boolean bonsaiGoState=0;
boolean bonsaiStopState=0;

boolean testStarted=0;

int cueLED = 5;
boolean cueLEDstate=0;

unsigned long randomWait, randomWait0, randomWait1, randomWait2;
int numWiggles;

int testNum=0;
int testVariation=1;
int servoCheck=0;

int i, j, n, p;

unsigned long currentTime;
unsigned long tButtonPoll, t2ButtonPoll;
unsigned long tRandomWait;
unsigned long tForagingComplete;
unsigned long tForagingStartRandom;


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

  // sets up I/O of pins
  pinMode(button0, INPUT_PULLUP);
  pinMode(bonsaiGoPin, INPUT_PULLUP);  
  pinMode(bonsaiStopPin, INPUT_PULLUP);  
  pinMode(cueLED, OUTPUT);

  pulseHigh = 511+pulseOffset;
  pulseLow = 526-pulseOffset;

  // use noise from A0 to seed random number generator
  randomSeed(analogRead(A0));

  // Prints to the Serial Monitor window: 
  // Start Serial Communication. 
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println(" ");
  Serial.println("__START SERIAL SIGNAL__ ");
  Serial.println(" ");    Serial.println(" ");   Serial.println(" ");
  Serial.println("Please position arm vertically at 'Noon' position ('start' position).");
  Serial.println("You have 5 seconds before servo verification checks begin.");
  Serial.println(" ");      

  stopServoPulses(); 
  Serial.println("**Servo Pulses Stopped - Free to rotate Servo manually.");
  Serial.println(" ");
  
  delay(5000);
  
  // if this is the first time loop is running, run servo verification 
  if(servoCheck==0) {
    servoVerification();
    servoCheck=1;
  }
  
  stopServoPulses();
  Serial.println(" "); 
  Serial.println("**Servo Pulses Stopped - Free to rotate Servo manually.");
  Serial.println("Please reposition arm to start position.");
  button0Prev = HIGH;
  button0Prev2Sec = HIGH;
  Serial.println("Hold button down for 2s to start Foraging test.  Press button once to stop test."); 
  Serial.println(" ");  
  
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
  currentTime = millis();
  Serial.println(currentTime);
  servoWiggle();
  currentTime = millis();
  Serial.println(currentTime);
  delay(3000);

  /***** 360 degree rotation *****/
  Serial.println("  ");
  Serial.println("Starting 360 Rotation Function");  
  ccw360();            // Rotate Counter Clockwise 360 degrees
  delay(2000);         // Pause prior next function test
  cw360();             // Rotate Clockwise 360 degrees
  delay(3000);

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
  delay(3000);

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
  delay(3000);
  
  /*** 45 degree rotation (L/R), PAUSE,  45 degree rotation (R/L) ***/
  Serial.println("  ");
  Serial.println("Starting 90+Pause+!90 Function"); 
  Serial.println("  ");
  ccw90();            // Rotate Counter Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  cw90();            // Rotate Counter Clockwise 180 degrees
  delay(2000);         // Assumuming rotates at 1Hz

  cw90();             // Rotate Clockwise 180 degrees
  delay(1000);         // 1sec pause at 6oClock
  ccw90();             // Rotate Clockwise 180 degrees
  delay(3000);

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
  for(i=0; i<1; i++) {
    moveServo(0, 490); //dance left
    delay(random(135, 195));
    stopServoPulses();
    delay(random(50,100));
    moveServo(0, 540); //dance right    
    delay(random(200,220));
    stopServoPulses();
    delay(random(50,100));
    moveServo(0, 490); //dance left
    delay(random(135, 195));
    stopServoPulses();
    delay(random(2000,4000));
  }
  moveServo(0, 511); //return to neutral
}


/***********************************************/
/*                                             */
/*  cw360                                      */
/*                                             */
/***********************************************/
void cw360() {
  // Rotate 360 Clockwise.
  moveServo(0, pulseLow); //Move the servo on channel 0 to an angle of +45 degrees
  delay(1035);
  moveServo(0, 511);  //Move the servo on channel 0 to an angle of 0 degrees
}


/***********************************************/
/*                                             */
/*  ccw360                                     */
/*                                             */
/***********************************************/
void ccw360() {
  // Rotate 360 Counter Clockwise.
  moveServo(0, pulseHigh);    // Move the servo on channel 0 to an angle of -45 degrees
  delay(985);         // Assumuming rotates at 1Hz
  moveServo(0, 511);  // Move the servo on channel 0 to an angle of 0 degrees
}


/***********************************************/
/*                                             */
/*  cw180                                      */
/*                                             */
/***********************************************/
void cw180() {
  // Rotate 180 Clockwise.
  moveServo(0, pulseLow);   // Move the servo on channel 0 to an angle of -45 degrees
  delay(545);            // Assumuming rotates at 1Hz
  moveServo(0, 511);    // Move the servo on channel 0 to an angle of 0 degrees
}


/***********************************************/
/*                                             */
/*  ccw180                                     */
/*                                             */
/***********************************************/
void ccw180() {
  // Rotate 180 Counter Clockwise.
  moveServo(0, pulseHigh);     // Move the servo on channel 0 to an angle of -45 degrees
  delay(495);           // Assumuming rotates at 1Hz
  moveServo(0, 511);   // Move the servo on channel 0 to an angle of 0 degrees
}

/***********************************************/
/*                                             */
/*  cw90                                      */
/*                                             */
/***********************************************/
void cw90() {
  // Rotate 180 Clockwise.
  moveServo(0, pulseLow);   // Move the servo on channel 0 to an angle of -45 degrees
  delay(295);            // Assumuming rotates at 1Hz
  moveServo(0, 511);    // Move the servo on channel 0 to an angle of 0 degrees
}


/***********************************************/
/*                                             */
/*  ccw90                                    */
/*                                             */
/***********************************************/
void ccw90() {
  // Rotate 180 Counter Clockwise.
  moveServo(0, pulseHigh);     // Move the servo on channel 0 to an angle of -45 degrees
  delay(230);           // Assumuming rotates at 1Hz
  moveServo(0, 511);   // Move the servo on channel 0 to an angle of 0 degrees
}

/***********************************************/
/*                                             */
/*  Program Loop                               */
/*                                             */
/***********************************************/
void loop() {
  
  // Start Foraging Test once button is pressed
  currentTime = millis();
 
/*  bonsaiGoState=digitalRead(bonsaiGoPin);
  bonsaiStopState=digitalRead(bonsaiStopPin);
  if(bonsaiGoState==0) {
    testStarted=1;  // Notes that the test has been manually started.
    foraging();    
  } */
  //Serial.println("ping");
  if (currentTime > tButtonPoll+10) {
    tButtonPoll=currentTime;
    button0Val = digitalRead(button0);
    button0Prev = button0Val;
    
    if (currentTime > t2ButtonPoll+2000) {
      t2ButtonPoll=currentTime;
      button0Prev2Sec = button0Prev;
      /*
      Serial.println("time");
      Serial.println(t2ButtonPoll);
      Serial.println(button0Val);
      Serial.println(button0Prev);
      Serial.println(button0Prev2Sec);
      */
    }  
    
    if (button0Val==LOW && button0Prev==LOW && button0Prev2Sec==HIGH) {
      testStarted=0;
      digitalWrite(cueLED, LOW);  
      stopServoPulses(); 
      //Serial.println("**Servo Pulses Stopped - Free to rotate Servo manually.");
      Serial.println("  **Press and hold button to start next foraging trial.**  ");  
      button0Val=HIGH;
      button0Prev=HIGH; 
    }
    
    if (button0Val==LOW && button0Prev==LOW && button0Prev2Sec==LOW) {  //Determine if button was pressed and held.
      testStarted=1;  // Notes that the test has been manually started. 
      button0Prev2Sec=HIGH;
      button0Prev=HIGH;
      button0Val=HIGH;
      foraging();
    }
  }
  
   
  /*
  Serial.println("val");
  Serial.println(button0Val);
  Serial.println("previous, 2 sec");
  Serial.println(button0Prev2Sec); 
  */
  if(testStarted && (currentTime >= tForagingStartRandom)) {
    foraging();
  }
  
}

/***********************************************/
/*                                             */
/*  Foraging                                   */
/*                                             */
/***********************************************/
void foraging() {
  Serial.println(" ");
  Serial.println("--FORAGING TEST ACTIVATED--");
  Serial.println(" ");
  digitalWrite(cueLED, HIGH);
  delay(2000);
  cw90();
  numWiggles = 3;        // 1 "wiggle" =~3seconds
  /*
  numWiggles = 3;  // day 2 of foraging, ~9 seconds
  numWiggles = 2;  // day 3 of foraging, ~6 seconds
  numWiggles = 1;  // day 4 of foraging, ~3 seconds
  */
  for(j=0; j<numWiggles; j++) {
    servoWiggle();
    //if button pressed, break out of loop
    //Serial.println("WIGGLE ping");
    currentTime=millis();
    if (currentTime > tButtonPoll+10) {
      tButtonPoll=currentTime;
      button0Val = digitalRead(button0);
      button0Prev = button0Val;
      
      if (currentTime > t2ButtonPoll+3000) {
        t2ButtonPoll=currentTime;
        button0Prev2Sec = button0Prev;
      }  
      
      if (button0Val==LOW && button0Prev==LOW && button0Prev2Sec==LOW) {
        testStarted=0;
        digitalWrite(cueLED, LOW);  
        stopServoPulses();
        Serial.println(" "); 
        Serial.println("Successful Catch!");
        Serial.println("**Servo Pulses Stopped - Free to rotate Servo manually.");
        Serial.println(" ");
        button0Val=HIGH;
        button0Prev=HIGH;
        button0Prev2Sec=HIGH;   
        break;  
      }
    }
    /*
    bonsaiStopState=digitalRead(bonsaiStopPin);
    if(bonsaiStopState==0) {
      testStarted=0;
      digitalWrite(bonsaiStopPin, HIGH);
      loop();       
    } */
  } 
  if (testStarted==1){
      digitalWrite(cueLED, LOW);  
      ccw90();
      Serial.println("No Catch.");      
      tForagingComplete=millis();
      stopServoPulses();
      
      randomWait0=random(30, 40);
      randomWait1=random(30, 50);
      randomWait2=random(30, 60);
      randomWait = (randomWait0 + randomWait1 + randomWait2)/3;
      tForagingStartRandom = tForagingComplete + randomWait*1000; 
      Serial.print("Next test begins in "); Serial.print(randomWait);
      Serial.println(" seconds.");
      Serial.println("**Servo Pulses Stopped - Free to rotate Servo manually.");
      Serial.println(" ");
  }
}


/***********************************************/
/*                                             */
/*  Move Servo Function - via Potentiometer  */
/*                                             */
/***********************************************/
void moveServo(int ServoChannel, int target) {
  //1000us PWM pulse represents a servo angle of 0 degrees.
  //2000us PWM pulse represents a servo angele of 90 degrees.
  //These values could vary based on the servo you use, check your servo's 
  //spec documentation and verify what PWM pulses are needed to move it.

  byte serialBytes[4]; //Create the byte array object that will hold the communication packet.
  target = map(target, 0, 1023, 4000, 8000); //Map the target angle to the corresponding PWM pulse.
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
  byte serialBytes[4]; //Create the byte array object that will hold the communication packet.
  serialBytes[0] = 0x84;           // Command byte: Set Target.
  serialBytes[1] = 0x00;           // Hard Code Servo_Channel_0
  serialBytes[2] = 0x00;           // Second byte holds the lower 7 bits of target.
  serialBytes[3] = 0x00;           // Third byte holds the bits 7-13 of target.
  maestroSerial.write(serialBytes, sizeof(serialBytes)); //Write the byte array to the serial port.
}



