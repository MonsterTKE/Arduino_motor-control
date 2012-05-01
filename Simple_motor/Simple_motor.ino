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

unsigned long loopCounter = 0; //Loop counter to refresh screen.
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
int jogSlope = 4;

//*************************HALL EFFECT, ISR variables*********************
volatile unsigned int tempHallCount = 0;
volatile unsigned int hallInterCount = 0;
volatile boolean tripped;
volatile boolean tripCount;

//************************Performance timing *****************************
unsigned long time = 0;

//************************PWM Stuff***************************************
int pwmSlope = 255;

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

  analogWrite(pwmPin, 100);
}

void loop() {



  int inputLeft = digitalRead(leftRed);
  int inputRight = digitalRead(rightRed);
  int menuButton = digitalRead(greenMenu);


  if (inputRight && !tripped) {
    jogMode_run(CCWpin, rightRed);
  }

  if (inputLeft && !tripped) {
    jogMode_run(CWpin, leftRed);
  }
  if (green.isHit()) {
    hallInterCount = 0;
    loopCounter = 0;
  }
  if (yellow.isHit()) {
    lcd.clear();
  }
  if (tripped) {
    tripped = false;
    digitalWrite(CCWpin, LOW);
  }

  lcd.setCursor(0,0);
  lcd.print("Steps passed:");
  lcd.setCursor(14,0);
  lcd.print(hallInterCount);

  lcd.setCursor(0,1);
  lcd.print("Temp passed:");
  lcd.setCursor(14,1);
  lcd.print(tempHallCount);

  lcd.setCursor(0,2);
  lcd.print("delay");
  lcd.setCursor(14,2);
  lcd.print(jDelay);

  lcd.setCursor(0,3);
  lcd.print("loops:");
  lcd.setCursor(14,3);
  lcd.print(loopCounter);
}

void hallCount() {
  time = micros();
  hallInterCount = hallInterCount + 1;
  tripped = true;
  time = micros() - time;
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

int pwmRamp() {
  if (hallInterCount <=25) {
    pwmSlope = 100;
  }
  if (hallInterCount <= 25) {
    pwmSlope++;
  }
  return pwmSlope;
}

void jogMode_run(int direction, int button) {
  int inputLeft = digitalRead(leftRed);
  int inputRight = digitalRead(rightRed);

  int read = digitalRead(button);

  while (true) {
    digitalWrite(direction, HIGH);
    read = digitalRead(button);
    loopCounter++;
    if (!read) {
      digitalWrite(direction, LOW);
      jDelay = 500;
      hallInterCount = 0;
      break;
    }
    if (tripped && hallInterCount <= 16) {
      digitalWrite(direction, LOW);
      tripped = false;
      break;
    }
    if (hallInterCount >= 17 && !read) {
      tripped = false;
      jDelay = 500;
      hallInterCount = 0;
      break;
    }
  }

  read = digitalRead(button);

  lcd.clear();
  getJogDelay();
  delay(jDelay);
  tripped = false;
}