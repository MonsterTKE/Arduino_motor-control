/*
this is the beginning framework for lcd menus
 and hardware debounced buttons
 */

#include <SerialLCD.h>
#include <NewSoftSerial.h> //this is a must


SerialLCD slcd(11,12); //Initialize the lcd lib.

const int inPin1 = 4; //Button pins
const int inPin2 = 5; //**
const int inPin3 = 6;//**
const int inPin4 = 7; //End button pins


int counter = 0; //Loop counter to refresh screen.

int menuMode = 1; //Initialize the controller in setup mode.

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

}

void setupDefault() {
  int pinRead1 = digitalRead(inPin1);
  int menuButton = digitalRead(inPin2);
  int inputLeft = digitalRead(inPin3);
  int inputRight = digitalRead(inPin4);

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

  slcd.setCursor(0, 0);
  slcd.print("Jog Mode");
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC);
}

void slewMenuMode() {
  slcd.setCursor(0, 0);
  slcd.print("Slew Mode");
  slcd.setCursor(12, 1);
  slcd.print(menuMode, DEC); 
}

