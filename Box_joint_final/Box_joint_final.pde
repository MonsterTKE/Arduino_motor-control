/*
this is the advancing framework for lcd menus
 and hardware debounced buttons, Now includes Switch case tomfoolery.
 */

#include <SerialLCD.h>
#include <NewSoftSerial.h> //this is a must


SerialLCD slcd(11,12); //Initialize the lcd lib.

const int inPin1 = 4; //Button pins
const int inPin2 = 5; //**
const int inPin3 = 6;//**
const int inPin4 = 7; //End button pins

boolean menuButtonPressed = false;  //button states
boolean menuBefore = false;         //**
boolean leftButtonPressed = false;  //**
boolean leftBefore = false;         //**
boolean RightButtonPressed = false; //**
boolean rightBefore = false;        //end button states.

int loopCounter = 0; //Loop counter to refresh screen.
unsigned long lcdUpdate;
unsigned long lastLcdUpdate;

int menuMode = 0; //Initialize the controller in setup mode.

void setup() {
  pinMode(inPin1, INPUT); 
  pinMode(inPin2, INPUT);
  pinMode(inPin3, INPUT);
  pinMode(inPin4, INPUT);
  Serial.begin(9600);
  slcd.begin();
  slcd.backlight();
}

void loop() {
  int pinRead1 = digitalRead(inPin1);
  int menuButton = digitalRead(inPin2);
  int inputLeft = digitalRead(inPin3);
  int inputRight = digitalRead(inPin4);

  if (menuButton == HIGH){
    menuMode++;
    delay(100);
	slcd.clear();	
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

void setupDefault() {
  int pinRead1 = digitalRead(inPin1);
  int menuButton = digitalRead(inPin2);
  int inputLeft = digitalRead(inPin3);
  int inputRight = digitalRead(inPin4);

  if (lcdRefreshOK()) {
    slcd.clear();
  }
  slcd.setCursor(0, 0);
  slcd.print("Setup Mode");
  slcd.setCursor(0, 1);
  slcd.print(pinRead1, DEC);
  slcd.setCursor(3, 1);
  slcd.print(menuButton, DEC);
  slcd.setCursor(6, 1);
  slcd.print(inputLeft, DEC);
  slcd.setCursor(9, 1);
  slcd.print(inputRight, DEC);
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC); 
}

void jogMenuMode() {
  int inputLeft = digitalRead(inPin3);
  int inputRight = digitalRead(inPin4);

  if (lcdRefreshOK()) {
    slcd.clear();
  }
  slcd.setCursor(0, 0);
  slcd.print("Jog Mode");
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC);
}

void slewMenuMode() {
  if (lcdRefreshOK()) {
    slcd.clear();
  }
  slcd.setCursor(0, 0);
  slcd.print("Slew Mode");
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC); 
}

boolean lcdRefreshOK() { //Refresh the lcd screen, Still need to find a better way to do this.
  boolean OK = false;
  lcdUpdate = millis();
  if (lcdUpdate > lastLcdUpdate) {
    lastLcdUpdate = lcdUpdate + 1000;
    OK = true;
  } 
  return OK;
}
