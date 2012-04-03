/*
Framework to handle button presses in a logical way.
 */

#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must


SerialLCD slcd(11,12); //Initialize the lcd lib.

const int inPin1 = 5; //right red button
const int inPin2 = 4; //left red button
const int inPin3 = 3; //green menu button
const int inPin4 = 8; //yellow enter button, I/O 2
const int inPin5 = 7; //left limit switch. Green/White twisted pair.
const int inPin6 = 6; //right limit switch. Blue/White twisted pair.

//const int Hallpin = 2;               // wired to Hall Effect sensor output
//const int CWpin  = 10;                // wired to MD01B pin INa
//const int CCWpin = 9;                // wired to MD01B pin INb


int loopCounter = 0; //Loop counter to refresh screen.
unsigned long lcdUpdate;
unsigned long lastLcdUpdate;
int refreshScreen = 0; //boolean counter to clear screen.

int menuMode = 0; //Initialize the controller in setup mode.

//measurement variables.
unsigned int targetSteps = 32; //inital target for slew mode and setup mode.
unsigned int testVar = 0; //counter for jogmode

int stepIncrements = 0;
int stepMultiplier = 1;

void setup() { //int yer inpins

  pinMode(inPin1, INPUT); 
  pinMode(inPin2, INPUT);
  pinMode(inPin3, INPUT);
  pinMode(inPin4, INPUT);
  pinMode(inPin5, INPUT);
  pinMode(inPin6, INPUT);
  
  Serial.begin(9600);
  slcd.begin();
  slcd.backlight();
}

void loop() {
	
}