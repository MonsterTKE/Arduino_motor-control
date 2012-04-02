/*Testing the pololu
 Version 1, first revolutions.
 */
int leftButton = 10;
int rightButton = 11;

int leftRead = 0;
int rightRead = 0;

int CWpin = 8;
int CCWpin = 9;
int PwmPin = 5;

int Var = 1;
int pwmRate = 255;
volatile int counter = 0;

void setup() {
  pinMode(CWpin, OUTPUT);
  pinMode(CCWpin, OUTPUT);
  pinMode(leftButton, INPUT);
  pinMode(leftButton, INPUT);
  Serial.begin(9600);
}

void loop() {
  leftRead = digitalRead(leftButton);
  rightRead = digitalRead(rightButton);
  if (leftRead == HIGH) {
    digitalWrite(CWpin, HIGH);
    Serial.print("Left Button: ");
      Serial.println(pwmRate);
  }
  else if (rightRead == HIGH) {
    digitalWrite(CCWpin, HIGH);
    Serial.print("Right Button: ");
      Serial.println(pwmRate);
  }
  else {
    digitalWrite(CCWpin, LOW);
    digitalWrite(CWpin, LOW);
  } 
  analogWrite(PwmPin, pwmRate);
  Serial.print("Counter: ");
    Serial.println(counter);
  }





