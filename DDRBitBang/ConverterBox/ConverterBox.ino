/*******************************************************************************
 * This file is part of PsxNewLib.                                             *
 *                                                                             *
 * Copyright (C) 2019-2020 by SukkoPera <software@sukkology.net>               *
 *                                                                             *
 * PsxNewLib is free software: you can redistribute it and/or                  *
 * modify it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * PsxNewLib is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with PsxNewLib. If not, see http://www.gnu.org/licenses.              *
 *******************************************************************************
 *
 * This sketch will dump to serial whatever is done on a PSX controller. It is
 * an excellent way to test that all buttons/sticks are read correctly.
 *
 * It's missing support for analog buttons, that will come in the future.
 *
 * This example drives the controller by bitbanging the protocol, there is
 * another similar one using the hardware SPI support.
 */

#include <DigitalIO.h>
#include <PsxControllerBitBang.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

  
// These can be changed freely when using the bitbanged protocol
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 9;

const byte PIN_PS2_ATT_2 = 6;
const byte PIN_PS2_CMD_2 = 5;
const byte PIN_PS2_DAT_2 = 4;
const byte PIN_PS2_CLK_2 = 7;


#define LED1 3
#define LED2 2

const byte PIN_BUTTONPRESS = A0;
const byte PIN_HAVECONTROLLER = A1;

const char buttonSelectName[] PROGMEM = "Select";
const char buttonL3Name[] PROGMEM = "L3";
const char buttonR3Name[] PROGMEM = "R3";
const char buttonStartName[] PROGMEM = "Start";
const char buttonUpName[] PROGMEM = "Up";
const char buttonRightName[] PROGMEM = "Right";
const char buttonDownName[] PROGMEM = "Down";
const char buttonLeftName[] PROGMEM = "Left";
const char buttonL2Name[] PROGMEM = "L2";
const char buttonR2Name[] PROGMEM = "R2";
const char buttonL1Name[] PROGMEM = "L1";
const char buttonR1Name[] PROGMEM = "R1";
const char buttonTriangleName[] PROGMEM = "Triangle";
const char buttonCircleName[] PROGMEM = "Circle";
const char buttonCrossName[] PROGMEM = "Cross";
const char buttonSquareName[] PROGMEM = "Square";

const char* const psxButtonNames[PSX_BUTTONS_NO] PROGMEM = {
  buttonSelectName,
  buttonL3Name,
  buttonR3Name,
  buttonStartName,
  buttonUpName,
  buttonRightName,
  buttonDownName,
  buttonLeftName,
  buttonL2Name,
  buttonR2Name,
  buttonL1Name,
  buttonR1Name,
  buttonTriangleName,
  buttonCircleName,
  buttonCrossName,
  buttonSquareName
};


/// DDR CONFIG \\\

//To add keys, refer to the USB HID Usage Tables: Chapter 10/Page 53
//https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

//By default, these will map to the standard StepMania key mappings.
#define KEY_ENTER   40
#define KEY_ESCAPE  41
#define KEY_RIGHT_ARROW 79
#define KEY_LEFT_ARROW  80
#define KEY_DOWN_ARROW  81
#define KEY_UP_ARROW    82
#define KEYPAD_2 90
#define KEYPAD_4 92
#define KEYPAD_6 94
#define KEYPAD_8 96
#define KEY_MINUS 45
#define KEYPAD_ENTER 88
#define NUM_KEYS 6 // #keys you are mapping PER PAD

uint8_t buf[8] = { 
  0 };   /* Keyboard report buffer */

int code[NUM_KEYS] = {
  KEY_ENTER,
  KEY_ESCAPE,
  KEY_RIGHT_ARROW,
  KEY_LEFT_ARROW,
  KEY_DOWN_ARROW,
  KEY_UP_ARROW
};
//Both codes must be in same order
int code2[NUM_KEYS] = {
  KEYPAD_ENTER,
  KEY_MINUS,
  KEYPAD_6,
  KEYPAD_4,
  KEYPAD_2,
  KEYPAD_8
};

//Reference PsxNewLib.h in library files for hex numbers
//that are assigned to each button, convert to decimaal
//Must be in same order as codes above
int key[NUM_KEYS] = {
   8,
   1,
   32,
   128,
   64,
   16
};

void toKeyboard(PsxButtons button, byte psx, bool& pushUpdate){

if (psx == 1)  {
  fastDigitalWrite(LED1, HIGH);
  for (byte n = 0; n < NUM_KEYS; n++){
    if (button == key[n]) {
      for (byte i = 2; i < 9; i++) {
        if(buf[i] == 0) { //check if other bits have been written, 6 possible from 2-8
         buf[i] = code[n];
//         Serial.println(i);
//         Serial.print(" modified");
//         Serial.write(buf,8);
           pushUpdate = true;
         break; 
         }
      } 
    }
  }
 } else if (psx == 2)  {
  fastDigitalWrite(LED2, HIGH);
  for (byte n = 0; n < NUM_KEYS; n++){
    if (button == key[n]) {
      for (byte i = 2; i < 9; i++) {
        if(buf[i] == 0) { //check if other bits have been written, 6 possible from 2-8
         buf[i] = code2[n];
//         Serial.println(i);
//         Serial.print(" modified");
        // Serial.write(buf,8);
        pushUpdate = true;
         break; 
         }
      } 
    }
  }
 }
}

void releaseKey(PsxButtons button, byte psx, bool& pushUpdate){
//  Serial.println("Released");
//  Serial.print(button);
  buf[0] = 0;
  buf[1] = 0;
  byte buttonCode;
  
 if (psx == 1)  {
  for (byte n = 0; n < NUM_KEYS; n++){
    if(button == key[n]){
      buttonCode = code[n]; 
//      Serial.println(buttonCode);
    }
   }
 } else if (psx == 2)  {
  for (byte n = 0; n < NUM_KEYS; n++){
    if(button == key[n]){
      buttonCode = code2[n]; 
//      Serial.println(buttonCode);
    }
   }
 }

  for (byte i = 2; i < 9; i++) {
        if(buf[i] == buttonCode) { //check if other bits have been written
         buf[i] = 0;
         // Serial.write(buf,8); 
         pushUpdate = true;
         break;
         
//          Serial.println(i);
//         Serial.print(" removed");
         } 
   }
   byte c = 0;
   for (byte i = 2; i < 9; i++){
    c += buf[i];
   }
   
   if (c==0 && psx == 1) fastDigitalWrite(LED1, LOW); //I'm not sure why c=1 for psx1 and c=0 for psx2
   else if (c==1 || c==0 && psx == 2) fastDigitalWrite(LED2, LOW);
}

byte psxButtonToIndex (PsxButtons psxButtons) {
  byte i;

  for (i = 0; i < PSX_BUTTONS_NO; ++i) {
    if (psxButtons & 0x01) {
      break;
    }

    psxButtons >>= 1U;
  }

  return i;
}

FlashStr getButtonName (PsxButtons psxButton) {
  FlashStr ret = F("");
  
  byte b = psxButtonToIndex (psxButton);
  if (b < PSX_BUTTONS_NO) {
    PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
    ret = PSTR_TO_F (bName);
  }

  return ret;
}

void dumpButtons (PsxButtons psxButtons) {
  
}

void dumpAnalog (const char *str, const byte x, const byte y) {
//  Serial.print (str);
//  Serial.print (F(" analog: x = "));
//  Serial.print (x);
//  Serial.print (F(", y = "));
//  Serial.println (y);
}



const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
  ctrlTypeUnknown,
  ctrlTypeDualShock,
  ctrlTypeDsWireless,
  ctrlTypeGuitHero,
  ctrlTypeOutOfBounds
};


PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;
PsxControllerBitBang<PIN_PS2_ATT_2, PIN_PS2_CMD_2, PIN_PS2_DAT_2, PIN_PS2_CLK_2> psx2;

boolean haveController = false;
boolean haveController2 = false;
 
void setup () {
  fastPinMode (PIN_BUTTONPRESS, OUTPUT);
  fastPinMode (PIN_HAVECONTROLLER, OUTPUT);
  fastPinMode (LED_BUILTIN, OUTPUT);
  fastPinMode (LED1, OUTPUT);
  fastPinMode (LED2, OUTPUT);
  
  delay (300);

  Serial.begin (9600);
//  Serial.println (F("Ready!"));
}

void checkButton(PsxButton button){
  bool pushUpdate = false;
    if (psx.buttonJustPressed (button)){
      toKeyboard(button, 1, pushUpdate);
     } 
     else if (psx.buttonJustReleased (button)){
      releaseKey(button, 1, pushUpdate);
     }
}

void checkButton2(PsxButton button){
  bool pushUpdate = false;
    if (psx2.buttonJustPressed (button)){
      toKeyboard(button, 2, pushUpdate);
     } 
     else if (psx2.buttonJustReleased (button)){
      releaseKey(button, 2, pushUpdate);
     }
}

PsxButton psxbutton2[NUM_KEYS] = {
  PSB_PAD_UP,
  PSB_PAD_DOWN,
  PSB_PAD_RIGHT,
  PSB_PAD_LEFT,
  PSB_SELECT,
  PSB_START
};
      
void loop () {
  static byte slx, sly, srx, sry;
  
  fastDigitalWrite (PIN_HAVECONTROLLER, haveController);
  
  if (!haveController) {
    if (psx.begin ()) {
//      Serial.println (F("Controller found!"));
      fastDigitalWrite(LED1, HIGH);
      delay (100);
      fastDigitalWrite(LED1, LOW);
      delay(100);
      fastDigitalWrite(LED1, HIGH);
      delay (100);
      fastDigitalWrite(LED1, LOW);
      if (!psx.enterConfigMode ()) {
//        Serial.println (F("Cannot enter config mode"));
      } else {
        PsxControllerType ctype = psx.getControllerType ();
        PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
//        Serial.print (F("Controller Type is: "));
//        Serial.println (PSTR_TO_F (cname));

        if (!psx.enableAnalogSticks ()) {
//          Serial.println (F("Cannot enable analog sticks"));
        }
        
        //~ if (!psx.setAnalogMode (fsalse)) {
          //~ Serial.println (F("Cannot disable analog mode"));
        //~ }
        //~ delay (10);
        
        if (!psx.enableAnalogButtons ()) {
//          Serial.println (F("Cannot enable analog buttons"));
        }
        
        if (!psx.exitConfigMode ()) {
//          Serial.println (F("Cannot exit config mode"));
        }
      }
      
      haveController = true;
    }
  } else {
    if (!psx.read ()) {
//      Serial.println (F("Controller lost :("));
      haveController = false;
      fastDigitalWrite(LED1, HIGH);
      delay (100);
      fastDigitalWrite(LED1, LOW);
      delay(100);
      fastDigitalWrite(LED1, HIGH);
     delay (100);
      fastDigitalWrite(LED1, LOW);
    } else {

      /*
      checkButton(PSB_PAD_UP);

      checkButton(PSB_PAD_DOWN);
     
      checkButton(PSB_PAD_RIGHT);
     
      checkButton(PSB_PAD_LEFT);
     
      checkButton(PSB_SELECT);
     
      checkButton(PSB_START);      
*/

      bool pushUpdate = false;
      
      for (byte b = 0; b < NUM_KEYS; b++) {

        PsxButton button = psxbutton2[b];
      if (psx.buttonJustPressed (button)){
      toKeyboard(button, 1, pushUpdate);
     } 
     else if (psx.buttonJustReleased (button)){
      releaseKey(button, 1, pushUpdate);
     }
      }
      if (pushUpdate) {
        Serial.write(buf,8);
      }
     
//      byte lx, ly;
//      psx.getLeftAnalog (lx, ly);
//      if (lx != slx || ly != sly) {
//        dumpAnalog ("Left", lx, ly);
//        slx = lx;
//        sly = ly;
//      }
//
//      byte rx, ry;
//      psx.getRightAnalog (rx, ry);
//      if (rx != srx || ry != sry) {
//        dumpAnalog ("Right", rx, ry);
//        srx = rx;
//        sry = ry;
//      }
    }
  }
 

 if (!haveController2) {
   if (psx2.begin ()) {
//      Serial.println (F("Controller2 found!"));
      fastDigitalWrite(LED2, HIGH);
      delay (100);
      fastDigitalWrite(LED2, LOW);
      delay(100);
      fastDigitalWrite(LED2, HIGH);
      delay (100);
      fastDigitalWrite(LED2, LOW);
      if (!psx2.enterConfigMode ()) {
//        Serial.println (F("Cannot enter config mode"));
      } else {
        PsxControllerType ctype = psx2.getControllerType ();
        PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
//        Serial.print (F("Controller Type is: "));
//        Serial.println (PSTR_TO_F (cname));

        if (!psx2.enableAnalogSticks ()) {
//          Serial.println (F("Cannot enable analog sticks"));
        }
        
        //~ if (!psx2.setAnalogMode (false)) {
          //~ Serial.println (F("Cannot disable analog mode"));
        //~ }
        //~ delay (10);
        
        if (!psx2.enableAnalogButtons ()) {
//          Serial.println (F("Cannot enable analog buttons"));
        }
        
        if (!psx2.exitConfigMode ()) {
//          Serial.println (F("Cannot exit config mode"));
        }
      }
      
      haveController2 = true;
    }
  } else {
    if (!psx2.read ()) {
//      Serial.println (F("Controller 2 lost :("));
      haveController2 = false;
      fastDigitalWrite(LED2, HIGH);
      delay (100);
      fastDigitalWrite(LED2, LOW);
      delay(100);
      fastDigitalWrite(LED2, HIGH);
     delay (100);
      fastDigitalWrite(LED2, LOW);
    } else {
 /*     
      checkButton2(PSB_PAD_UP);

      checkButton2(PSB_PAD_DOWN);
     
      checkButton2(PSB_PAD_RIGHT);
     
      checkButton2(PSB_PAD_LEFT);
     
      checkButton2(PSB_SELECT);
     
      checkButton2(PSB_START);
     */

      bool pushUpdate = false;
      
      for (byte b = 0; b < NUM_KEYS; b++) {

        PsxButton button = psxbutton2[b];
      if (psx.buttonJustPressed (button)){
      toKeyboard(button, 2, pushUpdate);
     } 
     else if (psx.buttonJustReleased (button)){
      releaseKey(button, 2, pushUpdate);
     }
      }
      if (pushUpdate) {
        Serial.write(buf,8);
      }
           
//      byte lx, ly;
//      psx.getLeftAnalog (lx, ly);
//      if (lx != slx || ly != sly) {
//        dumpAnalog ("Left", lx, ly);
//        slx = lx;
//        sly = ly;
//      }
//
//      byte rx, ry;
//      psx.getRightAnalog (rx, ry);
//      if (rx != srx || ry != sry) {
//        dumpAnalog ("Right", rx, ry);
//        srx = rx;
//        sry = ry;
//      }
    }
  }
  
  delay (1000 / 60);

}
