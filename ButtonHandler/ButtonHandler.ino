/*
Framework to handle button presses in a logical way, christ sakes that seemed whay more difficult that it looked.
But at least I have a class I can use elsewhere!
 */

#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must

SerialLCD slcd(11,12); //Initialize the lcd lib.

const int rightRed = 5; //right red button
const int leftRed = 4; //left red button
const int greenMenu = 3; //green menu button
const int yellowEnter = 8; //yellow enter button, I/O 2
const int leftLimit = 7; //left limit switch. Green/White twisted pair.
const int rightLimit = 6; //right limit switch. Blue/White twisted pair.

boolean oldButtonState = false;

  boolean inputLeft = digitalRead(leftRed);
  boolean inputRight = digitalRead(rightRed);
  boolean menuButton = digitalRead(greenMenu);
  boolean enterButton = digitalRead(yellowEnter);


//const int Hallpin = 2;               // wired to Hall Effect sensor output
//const int CWpin  = 10;                // wired to MD01B pin INa
//const int CCWpin = 9;                // wired to MD01B pin INb


int loopCounter = 0; //Loop counter to refresh screen.
unsigned long lcdUpdate;
unsigned long lastLcdUpdate;
int refreshScreen = 0; //boolean counter to clear screen.

int menuMode = 0; //Initialize the controller in setup mode.
int var = 0;
//measurement variables.
unsigned int targetSteps = 32; //inital target for slew mode and setup mode.
unsigned int testVar = 0; //counter for jogmode

int stepIncrements = 0;
int stepMultiplier = 1;

void setup() { //int yer inpins

  pinMode(rightRed, INPUT); 
  pinMode(leftRed, INPUT);
  pinMode(greenMenu, INPUT);
  pinMode(yellowEnter, INPUT);
  pinMode(leftLimit, INPUT);
  pinMode(rightLimit, INPUT);
  
  Serial.begin(9600);
  slcd.begin();
  slcd.backlight();
}

void loop() {
if (buttonHandler(greenMenu)) {
  slcd.setCursor(0,0);
  slcd.print("ok");
  slcd.setCursor(2,0);
  slcd.print(var, DEC);
}
    slcd.setCursor(0,1);
  slcd.print(oldButtonState, DEC);
}

/* This part seems to lose a button press if done quickly
Im not sure if it due to the code or due to the hysteresis 
in the schmitt trigger
*/

boolean buttonHandler(int buttonRead){

boolean buttonState = digitalRead(buttonRead);
boolean oldButton = oldButtonState;

if (buttonState && !oldButtonState) {
  var++;
  oldButtonState = buttonState;
}
else if (!buttonState) {
  oldButtonState = false;
}
}
