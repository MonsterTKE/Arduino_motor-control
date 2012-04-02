

const int inPin1 = 3; //Button pins
const int inPin2 = 4; //**
const int inPin3 = 5;//**
const int inPin4 = 6;
const int inPin5 = 7;
const int inPin6 = 8;
const int outPin1 = 13;
 //End button pins

int counter = 0;

void setup() {
  pinMode(inPin1, INPUT); 
  pinMode(inPin2, INPUT);
  pinMode(inPin3, INPUT);
  pinMode(inPin4, INPUT);
  pinMode(inPin5, INPUT);
  pinMode(inPin6, INPUT);
  pinMode(outPin1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int pinRead1 = digitalRead(inPin1);
  int pinRead2 = digitalRead(inPin2);
  int pinRead3 = digitalRead(inPin3);
  int pinRead4 = digitalRead(inPin4);
  int pinRead5 = digitalRead(inPin5);
  int pinRead6 = digitalRead(inPin6);

if (pinRead1)
{
  digitalWrite(outPin1,HIGH);
}
else if (pinRead2)
{
  digitalWrite(outPin1, HIGH);
}
else if (pinRead3)
{
  digitalWrite(outPin1, HIGH);
}
else if (pinRead4)
{
  digitalWrite(outPin1, HIGH);
}
else if (pinRead5)
{
  digitalWrite(outPin1, HIGH);
}
else if (pinRead6)
{
  digitalWrite(outPin1, HIGH);
}
else {
digitalWrite(outPin1,LOW);
}
}

