/*buttonHandler.h - Simple Library for incrementing a variable
on a buttonpress.
*/

#ifndef buttonHandler_h
#define buttonHandler_h

#include "Arduino.h"

class buttonHandler
{
 public:
 	buttonHandler(int buttonRead);
 private:
 	boolean buttonState;
 	boolean oldButton;
}


#endif