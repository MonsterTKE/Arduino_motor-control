

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long time = 0; 

  time = micros();
  //method to test
  testMethod(); 
 
 time = micros() - time;
  Serial.print("Test method required "); 
  Serial.print(time, DEC);
  Serial.println(" micro seconds.") ; 


}

void testMethod( ) {
  for (int i = 0; i < 10; i++) {
  Serial.println(i);
  
  if (i == 4)
    break;
}
}

void testMethod2() {
 byte test = 10;
if(test < 10)
    test = 20; 
}
