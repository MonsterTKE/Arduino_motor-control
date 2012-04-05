/* Controller program for lead screw based box joint jig. 
 Arduino control and Pololu MD01B motor driver. 
 See  http://www.flickr.com/photos/wb8nbs/sets/72157626932588778/
 also http://www.flickr.com/photos/wb8nbs/sets/72157622846581829/
 J. Harvey, wb8nbs at Prodigy dot Net
 
 Version 01 
 Code to read buttons for clockwise and counterclockwise, and a pot to set PWM
 Version 02
 Added LCD display. Using Analog pins for LCD data to free up digital pins
 Version 03
 Initial display from hall effect sensor
 Version 04
 Rearrange pins, ISR for Hall Effect, add MsTimer2 lib, begin menu & build Jog mode
 Version 05
 Clean up Jog mode, add slew mode, add LCD for jog and slew 
 Version 06
 Work on set up menu system
 Version 07
 Add analog debounce, step fault check, limit switch test, EEPROM target save
 Version 08
 Final code, rearrange pin numbers for sensible wiring
 */

#include <MsTimer2.h>                // Timer #2 library
#include <LiquidCrystal.h>           // the Liquid Crystal Display library
#include <EEPROM.h>                  // read/write bytes to the internal EEPROM 

// initialize LCD with the numbers of the interface pins
LiquidCrystal lcd(12, 10, 11, 8, 9, 6);// rainbow wire friendly arrangement

const byte EEPROM_ID = 0x55;         // used to determine if EEPROM is valid
const int ID_ADDR = 0;               // EEPROM address to store ID
const int SLEW_ADDR = 1;             // EEPROM address to store Target

const int leftButtonpin  = A5;       // left button, grounds when pressed
const int rightButtonpin = A4;       // right button, grounds when pressed
const int Hallpin = 2;               // wired to Hall Effect sensor output
const int CWpin  = 7;                // wired to MD01B pin INa
const int CCWpin = 4;                // wired to MD01B pin INb
const int Llimitpin = 3;             // Left limit microswitch
const int Rlimitpin = 5;             // Right limit microswitch

long LCDupdate;                      // Number of Milliseconds before an LCD update
long lastLCDupdate;                  // Milliseconds since previous update
int loopCount = 0;                   // Counts iterations

boolean LbuttonPressed = false;      // is left button pressed?
boolean Lbefore = false;             // previous left button state
boolean RbuttonPressed = false;      // is right button pressed?
boolean Rbefore = false;             // previous right button state

int loopMode = 1;                    // controls main loop mode, init to JOG
int jDelay;                          // initial time between jogs
int jogDelay = 500;                  // time between sucessive jogs
int jogSlope = 12;                   // sets slope of jog time decrease

volatile int pressCount;             // ISR accumlated count of sensor steps
int tmppressCount;                   // local copy of pressCount
int targetCount = 1;                 // # steps target
int tmptargetCount;                  // copy of target
volatile int Count;                  // ISR stores steps passed
volatile boolean Tripped;            // ISR indication that a step has passed
boolean tmpTripped = false;          // local copy of Tripped
volatile boolean countReached;       // ISR indication that count has been reached
boolean tmpcountReached = true;      // local copy of countReached
int stopDelay = 5;                   // ms to delay motor stop so sensor is centered 
volatile int stopTime;               // variable motor stop time delay

int subMenu = 0;                     // controls subMenu in set mode
int slewTarget = 32;                 // target count in slew mode init to 1/4"
/*********************************************************************/
void setup (){

  pinMode(Hallpin, INPUT);           // wired to Hall Effect sensor
  digitalWrite(Hallpin, HIGH);       // activate internal pullup
  pinMode(CWpin, OUTPUT);            // wired to MD01B motor driver INa
  digitalWrite(CWpin, LOW);          // Stop motor
  pinMode(CCWpin, OUTPUT);           // wired to MD01B motor driver INb
  digitalWrite(CCWpin, LOW);         // Stop motor
  pinMode(Llimitpin,INPUT);          // wired to LEFT limit switch
  pinMode(Rlimitpin,INPUT);          // wired to RIGHT limit switch

  lcd.begin(16, 2);                  // set up the LCD's columns and rows: 

  // set up ISR to count pulses coming in from the Hall Effect sensor
  //   attachInterrupt(0,Counter,RISING);
  attachInterrupt(0,Counter,FALLING);
  // for some reason, setting up the ISR makes all the variables 1
  // So init the variables here
  Tripped = false; 
  Count = 0;
  pressCount = 0; 
  countReached = false;

  // Get saved slewTarget if any (code from Cookbook pg 551)
  if(EEPROM.read(ID_ADDR) == EEPROM_ID) {  // has EEPROM been written before?
    // Read and assemble into a 16 bit INT
    slewTarget = word(EEPROM.read(SLEW_ADDR), EEPROM.read(SLEW_ADDR + 1));
  } 
  else {                             // this is a virgin EEPROM
    EEPROM.write(ID_ADDR, EEPROM_ID);// set id
    EEPROM.write(SLEW_ADDR, highByte(slewTarget));// set Target
    EEPROM.write(SLEW_ADDR + 1, lowByte(slewTarget));
  }
}

/*********************************************************************/
void loop() {
  /******  Test for limit switch activation  *****/
  if(!digitalRead(Llimitpin)) {      // read a side limit switch
    targetCount=1;                   // stop motor when next step completes
    tmppressCount = 0;               // set Jogs completed to zero
    LCDjogmode();                    // display to JOG
    lcd.setCursor(0,0);              // then overwrite first line
    lcd.print("** LEFT LIMIT **");
    delay(150);                      // rest a bit for less display flicker
    loopMode = 1;                    // force JOG mod
    jogDelay = 500;                  // will set up the display
  }

  if(!digitalRead(Rlimitpin)) {      // read b side limit switch
    targetCount=1;                   // stop motor when next step completes
    tmppressCount = 0;               // set Jogs completed to zero
    LCDjogmode();                    // display to JOG
    lcd.setCursor(0,0);              // then overwrite first line
    lcd.print("** RIGHT LIMIT *");
    delay(150);                      // rest a bit for less display flicker
    loopMode = 1;                    // force JOG mode
    jogDelay = 500;                  // will init the display
  }

  /******  Button Processing  *******/
  LbuttonPressed = !Button(leftButtonpin);
  if(LbuttonPressed && !Lbefore) {   // is this a *new* left button press?
    if(!Rbefore) {                   // IF right button *not* pressed,
      delay (200);                   // wait to see if right also pressed
    }
    if(tmpcountReached) {            // if slew is not in progress,
      pressCount = 0;                // re-init step counter
      tmppressCount = 0;             // init counter copy
    }
  }

  RbuttonPressed = !Button(rightButtonpin);
  if(RbuttonPressed && !Rbefore) {   // is this a *new* right button press?
    if(!Lbefore) {                   // if left button is *not* pressed,
      delay (200);                   // wait to see if left also pressed
      LbuttonPressed = !Button(leftButtonpin);
      Lbefore = LbuttonPressed;      // remember state of left button
    }
    if(tmpcountReached) {            // if slew is not in progress
      pressCount = 0;                // first indication of a button press
      tmppressCount = 0;             // init pressCount copy
    }
  }

  if(LbuttonPressed && RbuttonPressed) { // if both pressed,
    loopMode = 3;                        // Trigger SetUp mode
  } 
  if(!LbuttonPressed && !RbuttonPressed) { // -OR- Clean up if neither button pressed
    if(loopMode ==  1) {             // if in jog mode
      targetCount=1;                 // stop motor when next step completes
    }
    jogDelay = 500;
  }

  if (!LbuttonPressed) {             // if the left button is UN-pressed
    Lbefore = false;                 // clear the previous button press
  }

  if (!RbuttonPressed) {             // if the right button is UN-pressed
    Rbefore = false;                 // clear the previous button press
  }

  // dig info out of the ISR (code from the Cookbook)
  if(Tripped) {                      // if ISR has been triggered
    uint8_t oldSREG = SREG;          // save the interrupt register
    cli();                           // stop interrupts
    tmpTripped = true;               // records that a step was passed
    tmppressCount = pressCount;      // # steps since button was pressed
    tmpcountReached = countReached;  // flag that step count is done
    Tripped = false;                 // reset until next step tripped
    countReached = false;            // reset until next step tripped
    SREG = oldSREG;                  // restart interrupts
  }

  switch (loopMode) {                // Main Mode selector

  case 1:                            // ******  Jog Mode  ******
    LCDjogmode();                    // initial display
    if(tmpTripped || (Count == 0)) { // Step passed, check for changes
      if(LbuttonPressed || RbuttonPressed) {
        jogDelay = getJogDelay();    // jog speed based on how long held
      }
    }

    // is there anything to do?
    // block if limit switch is grounding
    if(LbuttonPressed && digitalRead(Llimitpin)) {
      Lbefore = true;                // remember button was pressed
      Count = 0;                     // init ISR step counter
      digitalWrite(CCWpin, HIGH);    // start motor Counter Clockwise
      if(jogDelay <= 20) {           // the highest speed
        targetCount = 2304;          // let it run continuously
      } 
      else {
        delay(jogDelay);             // or wait a bit
      }
    } 

    if(RbuttonPressed && digitalRead(Rlimitpin)) {
      Rbefore = true;                // remember button was pressed
      Count = 0;                     // init ISR step counter
      digitalWrite(CWpin, HIGH);     // start motor Clockwise
      if(jogDelay <= 20) {           // the highest speed
        targetCount = 2304;          // let it run continuously
      }
      else {
        delay(jogDelay);             // or wait a bit
      }
    }

    LCDjogmode();                    // update display
    tmpTripped = false;              // Done processing this step
    break;                           // end of case 1 Jog Mode

  case 2:                            // ******  Slew Mode  ******
    // is there anything to do?
    // block if limit switch is grounding or a slew is in progress
    if(LbuttonPressed && !Lbefore && digitalRead(Llimitpin) && tmpcountReached) {
      Lbefore = true;                // remember left was pressed 
      Count = 0;                     // init ISR step counter
      pressCount = 0;                // init ISR count since pressed
      countReached = false;          // init ISR count reached flag
      tmpcountReached = false;       // init ISR count reached flag copy
      digitalWrite(CCWpin, HIGH);    // motor runs Counter Clockwise
      targetCount = slewTarget;      // Run the number of steps requested
    } 

    if(RbuttonPressed && !Rbefore && digitalRead(Rlimitpin) && tmpcountReached) {
      Rbefore = true;                // remember right was pressed 
      Count = 0;                     // init ISR step counter
      pressCount = 0;                // init ISR count since pressed
      countReached = false;          // init ISR count reached flag
      tmpcountReached = false;       // init ISR count reached flag
      digitalWrite(CWpin, HIGH);     // motor runs Clockwise
      targetCount = slewTarget;      // Run the number of steps requested
    }

    LCDslewmode();                   // LCD display for mode 2
    tmpTripped = false;              // Done processing this step

    // IF there is no limit fault ...  then:
    if(digitalRead(Llimitpin) && digitalRead(Rlimitpin)) {
      // test for an overrun or underrun fault;
      if(tmpcountReached && (tmppressCount != slewTarget) && (tmppressCount > 0)) {
        // wait for both buttons pressed, then both released to clear the fault
        while(Button(leftButtonpin) || Button(rightButtonpin)) {
          LCDslewmode();             // print settings to LCD
          lcd.setCursor(0,0);        // overwrite the first line
          lcd.print("** STEP FAULT **");
        } 
        while(!Button(leftButtonpin) || !Button(rightButtonpin)) { 
        }
        tmpcountReached = true;      // initial condition for slew
        tmppressCount = 0;           // reset the fault
        Tripped = false;             // ensure ISR is reset
      }
    }
    break;                           // end of case 2

  case 3:                            // ******  Set Up/Menu Mode  ******
    targetCount = 1;                 // stop the motor normally if its running
    LCDsetUPmode();                  // display initial setup screen

    // wait for no buttons pressed
    while (!Button(leftButtonpin) || !Button(rightButtonpin)) {
    }                                // spin
    delay(100);                      // rest a bit

    // process LEFT button press to display and select a submenu
    while (!Button(leftButtonpin)) {
      subMenu = ++subMenu % 3;       // Cycle and Range check

      switch (subMenu) {
      case 0:
        lcd.setCursor(0,1);
        lcd.print(" Go to JOG  mode");
        tmppressCount = 0;           // inits the display
        loopMode = 1;                // set to JOG mode
        break;                       // end of subMenu case zero

      case 1:
        lcd.setCursor(0,1);
        lcd.print(" Go to SLEW mode");
        tmppressCount = 0;           // inits the display
        tmpcountReached = true;      // indicates slew not in progress
        loopMode = 2;                // set to SLEW mode
        break;                       // end of subMenu case 1

      case 2:
        lcd.setCursor(0,1);
        lcd.print(" Go to SET  mode");
        break;                       // end of subMenu case 2
      }                              // end of subMenu switch

      delay(750);                    // delay so menu doesn't cycle too fast
    }                                // end of while left still pressed 

    if(subMenu == 2) {               // Set Up Target
      LCDsetmode();                  // Initial LCD
      while(!Button(leftButtonpin) || !Button(rightButtonpin)) { 
      }                              // wait for no button press 

      while(true) {                  //  Hijack the main loop 
        if(!Button(leftButtonpin) || !Button(rightButtonpin)) {  //if either pressed
          for (loopCount = 0; loopCount < 20; loopCount++) {     // look for both
            delay(10);
            if(!Button(leftButtonpin) && !Button(rightButtonpin)) {  // both pressed
              while(!Button(leftButtonpin) || !Button(rightButtonpin)) {
              }                      // wait for both to be un-pressed
              subMenu = 0;
              loopMode = 2;          // on exit, default to Slew mode
              break;                 // exit to hijack loop
            }
          }
          if(subMenu != 2) {         // both buttons were pressed,
            tmpcountReached = true; 
            //  Save the current slewTarget to EEPROM 
            EEPROM.write(SLEW_ADDR, highByte(slewTarget));
            EEPROM.write(SLEW_ADDR + 1, lowByte(slewTarget));
            break;                   // exit to main loop
          }

          pressCount = 0;            // init num of iterations
          jogDelay = 500;            // initial delay half second

          while(!Button(leftButtonpin)) {        // Decrementing
            if(--slewTarget < 1) slewTarget = 1; // decrement and range chk.
            pressCount = pressCount + 1;
            if(jogDelay > 10) {
              jogDelay = getJogDelay();
            }
            delay(jogDelay);         // use the jog delay func. to time increments
            LCDsetmode();            // update the LCD
          }

          while(!Button(rightButtonpin)) {             // Incrementing
            if(++slewTarget > 2304) slewTarget = 2304; // increment and range chk.
            pressCount = pressCount + 1;
            if(jogDelay > 10) {
              jogDelay = getJogDelay();
            }
            delay(jogDelay);         // use the jog delay func. to time increments
            LCDsetmode();            // update the LCD
          }
        }                            // end of button pressed processing
      }                              // end of loop hijack
    }                                // end of submenu 2 section
    break;                           // end of main loop case 3

  default:                           // if here big time problem in main loop
    lcd.print("error");
    return;                          // exit the sketch
  }                                  // end of loopMode switch
}                                    // end of main loop

/***** Interrupt Service Routine *************************************/
/***** Counts transitions from the Hall Effect sensor ****************/
void Counter()
{
  pressCount = pressCount + 1;       // # steps with this button pressed
  // if target Count is reached, stop the motor after a delay
  if(++Count >= targetCount) {
    countReached = true;             // informs main count is done
    // if only 1 step then motor hasn't come up to full speed
    // so double the delay
    if(targetCount == 1) {
      stopTime = stopDelay*2;    
    }
    else {
      stopTime = stopDelay;          // assume motor is at full speed
    }
    // short delay, use interrupt timer 2 library so loop can continue
    MsTimer2::set(stopTime,stopMotor);
    MsTimer2::start();
  }
  Tripped = true;                    // informs main a step has been passed
}

/***** Stop the motor when MsTimer2 expires **************************/
void stopMotor(){
  digitalWrite(CWpin, LOW);          // dont know which dir. so stop both
  digitalWrite(CCWpin, LOW); 
  MsTimer2::stop();                  // done with this delay
}

/***** Allow LCD to update only every 150 millisec *******************/
boolean OKtoDisplay() {
  boolean OK = false;
  LCDupdate = millis();              // get current timer
  if(LCDupdate > lastLCDupdate) {
    lastLCDupdate = LCDupdate + 150; // when next update is due
    OK = true;
  }
  return OK;
}

/***** Liquid Crystal Display for JOG mode ***************************/
void LCDjogmode() {
  if(OKtoDisplay()) {
    lcd.clear();
    lcd.print ("Step/Sec   Jogs");

    if(jogDelay > 20) {
      lcd.setCursor(2,1);            // 3th character, second line
      lcd.print(1000/jogDelay);      // convert to Jogs per second
    } 
    else {
      lcd.setCursor(0,1);            // 1st character, second line
      lcd.print("Continuous");
    }
    lcd.setCursor(11,1);
    lcd.print(tmppressCount);        // Steps since button was pressed
  }
}

/***** Liquid Crystal Display for SLEW mode **************************/
void LCDslewmode() {
  if(OKtoDisplay()) {
    lcd.clear();
    lcd.print(" Steps   Target");
    lcd.setCursor(1,1);
    lcd.print(tmppressCount);        // number of steps since slew start
    lcd.setCursor(9,1);
    lcd.print(slewTarget);           // number of steps requested
  }
}
/***** Liquid Crystal Display for SETUP mode *************************/
void LCDsetUPmode() {
  if(OKtoDisplay()) {
    lcd.clear();
    lcd.print ("Change MODE:    "); 
    lcd.setCursor(0,1);
    lcd.print("Left to cycle");
  }
}

/***** Liquid Crystal Display for SET mode ***************************/
void LCDsetmode() {
  LCDupdate = millis();              // get current timer
  if(LCDupdate > lastLCDupdate) {
    lastLCDupdate = LCDupdate + 150; // when next update is due
    lcd.clear();
    lcd.print("Distance   Steps");
    lcd.setCursor(0,1);
    lcd.print((float(slewTarget) * 0.0078125),7); // 1/128 inch
    lcd.setCursor(11,1);
    lcd.print(slewTarget);
  }
}

/***** Decrement the jog delay ***************************************/
int getJogDelay() {
  // speed up the jog based on how long  a button has been held
  if(pressCount <= 1) {              // init to half a second
    jDelay = 500;
  }
  if(jDelay > 11) {            
    jDelay = jDelay - jDelay/jogSlope;// nice logrythmic decrease
  } 
  else {
    jDelay = 10;                     // set a 10 ms lower delay limit
  }
  return jDelay;
}

/***** function to read analog pins as debounced digital inputs ******/
// has 33 percent hysteresis
boolean Button(int Bpin) {
  int level = 600;                   // middle of dead zone
  while(level > 400 && level < 800) {// in dead zone
    level = analogRead(Bpin);        // try again
  }
  if(level >= 800) {
    return HIGH;                     // in upper third
  } 
  else {
    return LOW;                      // in lower third
  }
}

