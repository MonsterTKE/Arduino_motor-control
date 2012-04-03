#include "Arduino.h"
#include "buttonHandler.h"

boolean buttonHandler::buttonHandler(boolean buttonRead) {

boolean buttonState = digitalRead(buttonRead);
boolean oldButton = oldButtonState;

if (buttonState && !oldButtonState) {
  var++;
  oldButtonState = buttonState;
}
else if (!buttonState) {
  oldButtonState = false;
}
}
