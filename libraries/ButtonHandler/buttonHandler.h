/*buttonHandler.h - Simple Library for incrementing a variable
on a buttonpress.
*/

#ifndef buttonHandler_h
#define buttonHandler_h

#include "Arduino.h"

class buttonHandler
{
 public:
 	Button(boolean buttonRead)
 	boolean buttonHandler();
 private:
 	boolean buttonState;
 	boolean oldButton;
};


#endif