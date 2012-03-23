/*
this is the advancing framework for lcd menus
 and hardware debounced buttons, Now includes Switch case tomfoolery/and other stuff
 */

#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must


SerialLCD slcd(11,12); //Initialize the lcd lib.

const int inPin1 = 4; //right red button,  schmit I/O 4
const int inPin2 = 5; //left red button, I/O 5
const int inPin3 = 7; //green menu button, I/O 1
const int inPin4 = 6; //yellow enter button, I/O 2
const int inPin5 = 8; //left limit switch. I/O 6.
const int inPin6 = 9; //right limit switch, I/O 3.

//const int Hallpin = 2;               // wired to Hall Effect sensor output
//const int CWpin  = 10;                // wired to MD01B pin INa
//const int CCWpin = 3;                // wired to MD01B pin INb

boolean menuButtonPressed = false;  //button states
boolean menuBefore = false;         //**
boolean leftButtonPressed = false;  //**
boolean leftBefore = false;         //**
boolean RightButtonPressed = false; //**
boolean rightBefore = false;        //end button states.

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

void loop() { //high speed code goes here
mainMenu();
}

void mainMenu() { //this is the overall menu controller
	int inputLeft = digitalRead(inPin2);
	int inputRight = digitalRead(inPin1);
	int menuButton = digitalRead(inPin3);
	int enterButton = digitalRead(inPin4);

  if (menuButton == HIGH){
    noClear();
    menuMode++;
    delay(50);
		
  }
  if (menuMode == 3) {
    menuMode = 0; 
  } 

  switch (menuMode) {
  case 1:
    jogMenuMode();
    break;
  case 2:
    slewMenuMode();
    break;
  default:
    setupDefault();
    break;
  }
  loopCounter++;
}

void setupDefault() { //this is the default setup menu
	int inputRight = digitalRead(inPin1);
	int inputLeft = digitalRead(inPin2);
	int menuButton = digitalRead(inPin3);
	int enterButton = digitalRead(inPin4);
	double targetInches = targetSteps * .00390625;
	
	if (enterButton == HIGH) {
	stepIncrements++;
	//noClear();
	}
	if (stepIncrements == 3) {
	stepIncrements = 0;
	}
	switch(stepIncrements) {
	case 1:
	stepMultiplier = 10;
	break;
	case 2:
	stepMultiplier = 100;
	break;
	default:
	stepMultiplier = 1;
	break;
	}
	
  if (inputRight == HIGH) {
  targetSteps = targetSteps + stepMultiplier;
  refreshScreen =1;
  }
  else if (inputLeft == HIGH) {
  targetSteps = targetSteps - stepMultiplier;
  refreshScreen = 1;
  }

  	if (refreshScreen == 1 && !inputLeft && !inputRight) {
	noClearBottom();
	refreshScreen = 0;
	}
  slcd.setCursor(0, 0);
  slcd.print(" steps ");
  slcd.setCursor(7,0);
  slcd.print(stepIncrements, DEC);
  slcd.setCursor(8, 0);
  slcd.print(" inches ");
  slcd.setCursor(1, 1);
  slcd.print(targetSteps, DEC); 
  lcdPrintDouble(targetInches, 5, 9, 1);  
}

void jogMenuMode() { //This is the controller for jog mode
  slcd.setCursor(0, 0);
  slcd.print("Jog mode | steps");
  jogMenuControl();
}

void jogMenuControl() { //This is the action code for jog mode
	int inputRight = digitalRead(inPin1);
	int inputLeft = digitalRead(inPin2);
	
	
	if (inputLeft == HIGH) {
	testVar++;
	slcd.setCursor(8, 1);
	slcd.print("<--");
	refreshScreen = 1;
	}
	else if (inputRight == HIGH) {
	testVar++;
	slcd.setCursor(8, 1);
	slcd.print("-->");
	refreshScreen = 1;
	}
	else {
	testVar = 0;
	}
	if (refreshScreen == 1 && !inputLeft && !inputRight) {
	noClearBottom();
	refreshScreen = 0;
	}

  slcd.setCursor(12, 1);
  slcd.print(testVar, DEC);
  slcd.setCursor(0, 1);
  slcd.print(refreshScreen, DEC);
}

void slewMenuMode() { //this is the slewing controller
  if (lcdRefreshOK()) {
    noClear;
  }
  slcd.setCursor(0, 0);
  slcd.print("Slew Mode        ");
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC); 
  slcd.setCursor(1, 1);
  slcd.print(targetSteps, DEC);
}

void slewMenuControl() { //this is the sction code for slew mode.

}

boolean lcdRefreshOK() { //Refresh the lcd screen, this is a better way, calls noClear()
  boolean OK = false;
  lcdUpdate = millis();
  if (lcdUpdate > lastLcdUpdate) {
    lastLcdUpdate = lcdUpdate + 1000;
    OK = true;
  } 
  return OK;
}

void noClear() { //prints blanks to the screen, looks way better that using slcd.clear().
slcd.setCursor(0,0);
slcd.print("                ");
slcd.setCursor(0,1);
slcd.print("                ");
}

void noClearBottom() { //does the same as noClear but only clears the bottom row.
slcd.setCursor(0,1);
slcd.print("                ");
}

 void lcdPrintDouble( double val, byte precision, int row, int col){ // example: printDouble( 3.1415, 2, 0, 1); // prints 3.14 (two decimal places)
  // prints val on a ver 0012 text lcd with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)
	slcd.setCursor(row,col);
  
  if(val < 0.0){
    slcd.print('-');
    val = -val;
  }

  slcd.print (val,DEC);  //prints the int part
  if( precision > 0) {
    slcd.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
  mult *=10;

    if(val >= 0)
 frac = (val - int(val)) * mult;
    else
 frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
 padding--;
    while(  padding--)
 slcd.print("0");
    slcd.print(frac,DEC) ;
  }
}