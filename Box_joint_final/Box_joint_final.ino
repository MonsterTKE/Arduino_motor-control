/*
this is the advancing framework for lcd menus
 and hardware debounced buttons, Now includes Switch case tomfoolery/and other stuff
 totally rebuilt circuitry to use SMD parts and anrduino nano mounted on same board.
 This removes alot of the wire clutter inside the box and makes it simpler to visualize 
 what is connected to what and what it is doing.
 */
//*********************LIBRARY IMPORTS*********************************
#include <Wire.h>
#include <LiquidTWI.h>
#include <Tap.h>

LiquidTWI lcd(0); //Initialize the lcd lib.

//**********************INPUT PINS**************************************

const int rightRed = 5; //right red button
const int leftRed = 4; //left red button
const int greenMenu = 3; //green menu button
const int yellowEnter = 8; //yellow enter button, I/O 2
const int leftLimit = 7; //left limit switch. Green/White twisted pair.
const int rightLimit = 6; //right limit switch. Blue/White twisted pair.

//***********************OUTPUT PINS************************************

const int pwmPin = 9;
const int Hallpin = 2;               // wired to Hall Effect sensor output
const int CWpin  = A1;                // wired to MD01B pin INa
const int CCWpin = A0;                // wired to MD01B pin INb

//*************************TAP lib constructors*************************
Tap green(greenMenu);
Tap right_Rb(rightRed);
Tap left_Rb(leftRed);
Tap yellow(yellowEnter);

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

  pinMode(rightRed, INPUT); 
  pinMode(leftRed, INPUT);
  pinMode(greenMenu, INPUT);
  pinMode(yellowEnter, INPUT);
  pinMode(leftLimit, INPUT);
  pinMode(rightLimit, INPUT);

  pinMode(pwmPin, OUTPUT);
  pinMode(CWpin, OUTPUT);
  pinMode(CCWpin, OUTPUT);

  lcd.begin(20,4);
  lcd.setBacklight(HIGH);
}

void loop() { //high speed code goes here
    mainMenu();
}

void mainMenu() { //this is the overall menu controller

  if (green.isHit()){
    lcd.clear();
    menuMode++;
    
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

  double targetInches = targetSteps * .00390625;

  if (yellow.isHit()) {
    stepIncrements++;
    lcd.setCursor(16,0);
    lcd.print("    ");
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

  if (right_Rb.isHit()) {
    targetSteps = targetSteps + stepMultiplier;
    refreshScreen =1;
  }
  else if (left_Rb.isHit()) {
    targetSteps = targetSteps - stepMultiplier;
    refreshScreen = 1;
  }

  if (refreshScreen == 1 && !left_Rb.isHit() && !right_Rb.isHit()) {
    lcd.setCursor(6,3);
    lcd.print("         ");
    lcd.setCursor(6, 2);
    lcd.print("      ");
    refreshScreen = 0;
  }
  lcd.setCursor(0,0);
  lcd.print("Setup");

    if (stepIncrements == 0){
    lcd.setCursor(7,0);
    lcd.print("Incr. x 1");
    }
      if (stepIncrements == 1){
      lcd.setCursor(7,0);
      lcd.print("Incr. x 10");
      }
        if (stepIncrements == 2){
        lcd.setCursor(7,0);
        lcd.print("Incr. x 100");
        }

  lcd.setCursor(0, 2);
  lcd.print("Step:");
  lcd.setCursor(6, 2);
  lcd.print(targetSteps, DEC);

  lcd.setCursor(0, 3);
  lcd.print("Inch:"); 
  lcd.setCursor(6,3);
  lcd.print(targetInches,4);  
}

void jogMenuMode() { //This is the controller for jog mode
  lcd.setCursor(0, 0);
  lcd.print("Jog mode | steps");
  jogMenuControl();
}

void slewMenuMode() { //this is the slewing controller
  if (lcdRefreshOK()) {
    noClear;
  }
  lcd.setCursor(0, 2);
  lcd.print("Slew Mode        ");
  lcd.setCursor(12, 1);
  lcd.print(menuMode, DEC); 
  lcd.setCursor(1, 1);
  lcd.print(targetSteps, DEC);
}

//*************************************** LCD HANDLERS **********************************

boolean lcdRefreshOK() { //Refresh the lcd screen, this is a better way, calls noClear()
  boolean OK = false;
  lcdUpdate = millis();
  if (lcdUpdate > lastLcdUpdate) {
    lastLcdUpdate = lcdUpdate + 1000;
    OK = true;
  } 
  return OK;
}

void noClear() { //prints blanks to the screen, looks way better that using lcd.clear().
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void noClearBottom() { //does the same as noClear but only clears the bottom row.
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void lcdPrintDouble( double val, byte precision, int row, int col){ // example: printDouble( 3.1415, 2, 0, 1); // prints 3.14 (two decimal places)
  // prints val on a ver 0012 text lcd with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)
  lcd.setCursor(row,col);

  if(val < 0.0){
    lcd.print('-');
    val = -val;
  }

  lcd.print (val,DEC);  //prints the int part
  if( precision > 0) {
    lcd.print("."); // print the decimal point
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
      lcd.print("0");
    lcd.print(frac,DEC) ;
  }
}


//***************** MOTOR HANDLING *************************

void rightLimitHandler() { //this controld the behavior of the system when the RIGHT limit switch is HIGH.
  // placeholder
}

void leftlimit() { //this controld the behavior of the system when the LEFT limit switch is HIGH.
  //placeholder
}

/*
void stopMotor(){ //This is to stop the motor
  digitalWrite(CWpin, LOW); 
  digitalWrite(CCWpin, LOW);
}
*/

void slewMenuControl() { //this is the action code for slew mode.

}

void jogMenuControl() { //This is the action code for jog mode

  boolean rightRed_b = digitalRead(rightRed);
  boolean leftRed_b = digitalRead(leftRed);

  if (left_Rb.isHit()) {
    testVar++;
    lcd.setCursor(8, 1);
    lcd.print("<--");
    refreshScreen = 1;
  }
  else if (right_Rb.isHit()) {
    testVar++;
    lcd.setCursor(8, 1);
    lcd.print("-->");
    refreshScreen = 1;
  }
  else {
    testVar = 0;
  }
  if (refreshScreen == 1 && !rightRed_b && !leftRed_b) {
    noClearBottom();
    refreshScreen = 0;
  }

  lcd.setCursor(12, 1);
  lcd.print(testVar, DEC);
  lcd.setCursor(0, 1);
  lcd.print(refreshScreen, DEC);
}