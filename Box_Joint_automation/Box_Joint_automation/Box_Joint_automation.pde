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

int counter = 0;

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
  int pinRead2 = digitalRead(inPin2);
  int pinRead3 = digitalRead(inPin3);
  int pinRead4 = digitalRead(inPin4);

  if (counter < 5) {
    if (pinRead1 == HIGH) {
      slcd.setCursor(0, 0);
      slcd.print("Pin one state: ");
      slcd.setCursor(0, 1);
      slcd.print(pinRead1, DEC);
      slcd.setCursor(12, 1);
      slcd.print(counter, DEC);
    }
    else if (pinRead2 == HIGH) {
      slcd.setCursor(0, 0);
      slcd.print("Pin two state: ");
      slcd.setCursor(0, 1);
      slcd.print(pinRead2, DEC);
      slcd.setCursor(12, 1);
      slcd.print(counter, DEC);
    }
    else if (pinRead3 == HIGH) {
      slcd.setCursor(0, 0);
      slcd.print("Pin three state: ");
      slcd.setCursor(0, 1);
      slcd.print(pinRead3, DEC);
      slcd.setCursor(12, 1);
      slcd.print(counter, DEC);
    }
    else if (pinRead4 == HIGH) {
      slcd.setCursor(0, 0);
      slcd.print("Pin four state: ");
      slcd.setCursor(0, 1);
      slcd.print(pinRead4, DEC);
      slcd.setCursor(12, 1);
      slcd.print(counter, DEC);
    }
    else {
      slcd.setCursor(0, 0);
      slcd.print("Waiting");
      slcd.setCursor(0, 1);
      slcd.print(pinRead1, DEC);
      slcd.setCursor(3, 1);
      slcd.print(pinRead2, DEC);
      slcd.setCursor(6, 1);
      slcd.print(pinRead3, DEC);
      slcd.setCursor(9, 1);
      slcd.print(pinRead4, DEC);
      slcd.setCursor(12, 1);
      slcd.print(counter, DEC);
    }
  }
  else {
    slcd.clear();
    counter = 0; 
  }
  counter++;
}






