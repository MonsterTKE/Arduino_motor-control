
#include <Tap.h>
#include <Wire.h>
#include <LiquidTWI.h>

LiquidTWI lcd(0);

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
//*************************HALL EFFECT, ISR variables*********************

volatile unsigned int tempHallCount = 0;
volatile unsigned int hallInterCount = 0;

void setup() { //int yer inpins

  pinMode(rightRed, INPUT); 
  pinMode(leftRed, INPUT);
  pinMode(greenMenu, INPUT);
  pinMode(yellowEnter, INPUT);
  pinMode(leftLimit, INPUT);
  pinMode(rightLimit, INPUT);

  pinMode(Hallpin, INPUT);

  pinMode(pwmPin, OUTPUT);
  pinMode(CWpin, OUTPUT);
  pinMode(CCWpin, OUTPUT);

  lcd.begin(20,4);
  lcd.setBacklight(HIGH);

  digitalWrite(Hallpin, HIGH);
  attachInterrupt(0, hallCount, FALLING);

  tempHallCount = 0;
  hallInterCount = 0;
  analogWrite(pwmPin, 155);
}

void loop() {
  int inputLeft = digitalRead(leftRed);
  int inputRight = digitalRead(rightRed);
  int menuButton = digitalRead(greenMenu);


  if (left_Rb.isHit()) {
    slewMode_c(CWpin);
  }
  else if (right_Rb.isHit()) {
    slewMode_c(CCWpin);
  }
  else if (menuButton == HIGH) {
    hallInterCount = 0;
    tempHallCount = 0;
    lcd.clear();
  }
  else if (hallInterCount == targetSteps) {
    stopMotor();
    hallInterCount = 0;
  }
  lcd.setCursor(0,0);
  lcd.print(inputRight, DEC);
  lcd.setCursor(2,0);
  lcd.print(inputLeft, DEC);
  lcd.setCursor(4,0);
  lcd.print(hallInterCount, DEC);
  lcd.setCursor(6,0);
  lcd.print(tempHallCount, DEC);
  lcd.setCursor(8,0);
  lcd.print(targetSteps, DEC);
}

void hallCount() {
  tempHallCount = hallInterCount;
  hallInterCount++;
}

void stopMotor(){ //This is to stop the motor
  digitalWrite(CWpin, LOW); 
  digitalWrite(CCWpin, LOW);
}

void slewMode_c(int direction) {

while (hallInterCount < targetSteps) {
  digitalWrite(direction, HIGH);
}
if (hallInterCount == targetSteps) {
  stopMotor();
  hallInterCount = 0;
  lcd.setCursor(8,0);
  lcd.print(targetSteps, DEC);
}
}