#include <EEPROM.h>

void setup() {
	Serial.begin(9600);
EEPROM.write(10, 255);
}

void loop() {

Serial.println(EEPROM.read(10));
delay(1000);

}