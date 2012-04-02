#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must

SerialLCD slcd(11,12); //Initialize the lcd lib.

const int inPin1 = 4; //right red button,  schmit I/O 4
const int inPin2 = 3; //left red button, I/O 5
const int inPin3 = 5; //green menu button, I/O 1
const int hallPin = 2;

const int outCCW = 9; //right red button,  schmit I/O 4
const int outCW = 10; //left red button, I/O 5

int targetSteps = 10;

volatile unsigned int hallInterCount = 0;
 volatile unsigned int tempHallCount = 0;
 
void setup() {
	pinMode(inPin1, INPUT); 
	pinMode(inPin2, INPUT);
	pinMode(inPin3, INPUT);
	pinMode(outCCW, OUTPUT); 
	pinMode(outCW, OUTPUT);
	
	pinMode(hallPin, INPUT);           // wired to Hall Effect sensor
    digitalWrite(hallPin, HIGH);
	
	attachInterrupt(0, hallCount, FALLING);
	
		Serial.begin(9600);
		slcd.begin();
		slcd.backlight();
		
		hallInterCount = 0;
		tempHallCount = 0;
}

void loop() {
  int inputLeft = digitalRead(inPin2);
  int inputRight = digitalRead(inPin1);
  int menuButton = digitalRead(inPin3);

  
  if (inputLeft == HIGH) {
  digitalWrite(outCW, HIGH);
  }
 else if (inputRight == HIGH) {
  digitalWrite(outCCW, HIGH);
  }
  else if (menuButton == HIGH) {
  hallInterCount = 0;
  tempHallCount = 0;
  slcd.clear();
  }
     else if (hallInterCount == targetSteps) {
  	digitalWrite(outCCW, LOW);
	digitalWrite(outCW, LOW);
	hallInterCount = 0;
    }
  slcd.setCursor(0,0);
  slcd.print(inputRight, DEC);
  slcd.setCursor(2,0);
  slcd.print(inputLeft, DEC);
  slcd.setCursor(4,0);
  slcd.print(hallInterCount, DEC);
  slcd.setCursor(6,0);
  slcd.print(tempHallCount, DEC);
}

void hallCount() {
tempHallCount = hallInterCount;
hallInterCount++;
}