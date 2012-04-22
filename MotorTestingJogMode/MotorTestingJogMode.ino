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

//*************************JOG TIMERS*************************************
int jDelay;                          // initial time between jogs
int jogDelay = 500;                  // time between sucessive jogs
int jogSlope = 12;

//*************************HALL EFFECT, ISR variables*********************
volatile unsigned int tempHallCount = 0;
volatile unsigned int hallInterCount = 0;
volatile boolean tripped;
volatile boolean tripCount;

//************************Performance timing *****************************

unsigned long currentMicros;

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

  digitalWrite(Hallpin, HIGH);
  attachInterrupt(0, hallCount, FALLING);

  tempHallCount = 0;
  hallInterCount = 0;
  tripped = false;
  tripCount = false;

  lcd.begin(20,4);
  lcd.setBacklight(HIGH);

  analogWrite(pwmPin, 255);
}

void loop() {
  int inputLeft = digitalRead(leftRed);
  int inputRight = digitalRead(rightRed);
  int menuButton = digitalRead(greenMenu);

  currentMicros = micros();

  if (right_Rb.isHit()) {
    stepOne();
  }
  if (left_Rb.isHit()) {
    digitalWrite(CCWpin, LOW);
  }
 lcd.setCursor(0,0);
 lcd.print(inputRight, DEC);
 lcd.setCursor(2,0);
 lcd.print(inputLeft, DEC);
 lcd.setCursor(4,0);
 lcd.print(hallInterCount, DEC);
 lcd.setCursor(6,1);
 lcd.print(tempHallCount, DEC);
 lcd.setCursor(8,2);
 lcd.print(targetSteps, DEC);
 lcd.setCursor(8,3);
 lcd.print(tripped, DEC);
}

void hallCount() {
  hallInterCount = hallInterCount + 1;
  tempHallCount = hallInterCount;
  tripped = true;
  if (hallInterCount >= targetSteps) {
    tripCount = true;
    hallInterCount = 0;
  }
}

void stopMotor(){ //This is to stop the motor
  digitalWrite(CWpin, LOW); 
  digitalWrite(CCWpin, LOW);
}

void jogMode_c(int direction, int button) {

boolean buttonPress = digitalRead(button);

if (buttonPress) {
  digitalWrite(direction, HIGH);
}
else {
  stopMotor();
}
}

int getJogDelay() {
  // speed up the jog based on how long  a button has been held
  if(hallInterCount <= 1) {              // init to half a second
    jDelay = 500;
  }
  if(jDelay > 11) {            
    jDelay = jDelay - jDelay/jogSlope;// nice logrythmic decrease
  } 
  else {
    jDelay = 10;                     // set a 10 ms lower delay limit
  }
  return jDelay;
}

void stepOne() {

unsigned long currentTime = currentMicros - micros()/1000;

  if (!tripCount) {
    if (targetSteps - hallInterCount < 10){
    analogWrite(pwmPin, 60);
    }
    if (targetSteps - hallInterCount > 10){
    analogWrite(pwmPin, 255);
    }
      digitalWrite(CWpin, HIGH);
      digitalWrite(CCWpin, LOW);
    } 
    if (tripCount) {
    digitalWrite(CWpin, LOW);
    tripCount = false;
    }

    lcd.setCursor(8,4);
    lcd.print(currentTime);

}


