#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must

SerialLCD slcd(11,12); //Initialize the lcd lib.

int floatEX = 2345657;
int floatWhy = 123454;
void setup() {
  Serial.begin(9600);
  slcd.begin();
  slcd.backlight();
}

void loop() {
lcdPrint("hello", 4, 0);
delay(1000);
lcdPrint("derp", 4, 0);
delay(1000);
}


 // Screw C++ strings, especially on embedded.
void lcdPrint(char *message, int x, int y) {
    char spaces[] = "                "; // 16 spaces
    char buf[17]; // needs to hold 16 chars + null
    strlcpy(buf, message, sizeof(buf)); // Copy the message into the buffer (up to 16 chars)
    strlcat(buf, spaces, sizeof(buf)); // Copy spaces into the rest of the buffer
    if (x || y)  slcd.setCursor(x, y); // Move the cursor
    slcd.print(buf); // Print
}
