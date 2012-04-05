#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must
#include <Tap.h>

SerialLCD slcd(11,12); //Initialize the lcd lib.

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

  Serial.begin(9600);
  slcd.begin();
  slcd.backlight();

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
    jogMode_c(CWpin);
  }
  else if (right_Rb.isHit()) {
    jogMode_c(CCWpin);
  }
  else if (menuButton == HIGH) {
    hallInterCount = 0;
    tempHallCount = 0;
    slcd.clear();
  }
  slcd.setCursor(0,0);
  slcd.print(inputRight, DEC);
  slcd.setCursor(2,0);
  slcd.print(inputLeft, DEC);
  slcd.setCursor(4,0);
  slcd.print(hallInterCount, DEC);
  slcd.setCursor(6,0);
  slcd.print(tempHallCount, DEC);
  slcd.setCursor(8,0);
  slcd.print(targetSteps, DEC);
}

void hallCount() {
  tempHallCount = hallInterCount;
  hallInterCount++;
}

void stopMotor(){ //This is to stop the motor
  digitalWrite(CWpin, LOW); 
  digitalWrite(CCWpin, LOW);
}

void jogMode_c(int direction) {

while (hallInterCount == 0) {
  digitalWrite(direction, HIGH);
}
if (hallInterCount == 1) {
  stopMotor();
  hallInterCount = 0;
  slcd.setCursor(8,0);
  slcd.print(targetSteps, DEC);
}
}
