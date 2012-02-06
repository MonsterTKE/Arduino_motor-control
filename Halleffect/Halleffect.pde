int hall = 2;
int led = 10;
int ledStatus = 0;
const int hallPin = 2;
volatile int ledCount = 1;

void setup() {
  pinMode(hall, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(hallPin, HIGH);

  attachInterrupt(0, countT, FALLING);

  Serial.begin(9600); 
}

void loop() {

  if (ledCount % 2 == 0) {
    Serial.print("hall value: ");
    Serial.println(ledCount);
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
}

void countT() {
  ledCount++; 
}



