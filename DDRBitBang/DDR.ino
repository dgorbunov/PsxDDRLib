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


//To add keys, refer to the USB HID Usage Tables: Chapter 10/Page 53
//https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
  
// These can be changed freely when using the bitbanged protocol
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 9;

//const byte PIN_PS2_ATT2 = 10;
//const byte PIN_PS2_CMD2 = 11;
//const byte PIN_PS2_DAT2 = 12;
//const byte PIN_PS2_CLK2 = 9; 


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

#define KEY_ENTER   40
#define KEY_ESCAPE  41
#define KEY_RIGHT_ARROW 79
#define KEY_LEFT_ARROW  80
#define KEY_DOWN_ARROW  81
#define KEY_UP_ARROW    82
#define NUM_KEYS 6 //total #keys you are mapping

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

//Reference PsxNewLib.h in library files for hex numbers
//that are assigned to each button, convert to decimaal
int key[NUM_KEYS] = {
   8,
   1,
   32,
   128,
   64,
   16
};


void toKeyboard(PsxButtons button){

fastDigitalWrite(LED_BUILTIN, HIGH);

  for (byte n = 0; n < NUM_KEYS; n++){
    if (button == key[n]) {
      for (byte i = 2; i < 9; i++) {
        if(buf[i] == 0) { //check if other bits have been written, 6 possible from 2-8
         buf[i] = code[n];
//         Serial.println(i);
//         Serial.print(" modified");
         Serial.write(buf,8);
         break; 
         }
      } 
    }
  }
}

void releaseKey(PsxButtons button){
//  Serial.println("Released");
//  Serial.print(button);
  buf[0] = 0;
  buf[1] = 0;
  int buttonCode;
  
  for (byte n = 0; n < NUM_KEYS; n++){
    if(button == key[n]){
      buttonCode = code[n]; 
//      Serial.println(buttonCode);
    }
   }

  for (byte i = 2; i < 9; i++) {
        if(buf[i] == buttonCode) { //check if other bits have been written
         buf[i] = 0;
         Serial.write(buf,8); 
         break;
         
//          Serial.println(i);
//         Serial.print(" removed");
         } 
   }
   int c = 0;
   for (byte i = 2; i < 9; i++){
    c += buf[i];
   }
   Serial.println(c);
   if (c==1) fastDigitalWrite(LED_BUILTIN, LOW);
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
//	Serial.print (str);
//	Serial.print (F(" analog: x = "));
//	Serial.print (x);
//	Serial.print (F(", y = "));
//	Serial.println (y);
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

boolean haveController = false;
 
void setup () {
	fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	fastPinMode (PIN_HAVECONTROLLER, OUTPUT);
  fastPinMode (LED_BUILTIN, OUTPUT);
	
	delay (300);

	Serial.begin (9600);
//	Serial.println (F("Ready!"));
}

void checkButton(PsxButton button){
    if (psx.buttonJustPressed (button)){
      toKeyboard(button);
     } 
     else if (psx.buttonJustReleased (button)){
      releaseKey(button);
     }
}
 
void loop () {
	static byte slx, sly, srx, sry;
	
	fastDigitalWrite (PIN_HAVECONTROLLER, haveController);
	
	if (!haveController) {
		if (psx.begin ()) {
//			Serial.println (F("Controller found!"));
      fastDigitalWrite(LED_BUILTIN, HIGH);
      delay (100);
      fastDigitalWrite(LED_BUILTIN, LOW);
      delay(100);
      fastDigitalWrite(LED_BUILTIN, HIGH);
			delay (100);
      fastDigitalWrite(LED_BUILTIN, LOW);
			if (!psx.enterConfigMode ()) {
//				Serial.println (F("Cannot enter config mode"));
			} else {
				PsxControllerType ctype = psx.getControllerType ();
				PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
//				Serial.print (F("Controller Type is: "));
//				Serial.println (PSTR_TO_F (cname));

				if (!psx.enableAnalogSticks ()) {
//					Serial.println (F("Cannot enable analog sticks"));
				}
				
				//~ if (!psx.setAnalogMode (false)) {
					//~ Serial.println (F("Cannot disable analog mode"));
				//~ }
				//~ delay (10);
				
				if (!psx.enableAnalogButtons ()) {
//					Serial.println (F("Cannot enable analog buttons"));
				}
				
				if (!psx.exitConfigMode ()) {
//					Serial.println (F("Cannot exit config mode"));
				}
			}
			
			haveController = true;
		}
	} else {
		if (!psx.read ()) {
//			Serial.println (F("Controller lost :("));
			haveController = false;
      fastDigitalWrite(LED_BUILTIN, HIGH);
      delay (100);
      fastDigitalWrite(LED_BUILTIN, LOW);
      delay(100);
      fastDigitalWrite(LED_BUILTIN, HIGH);
     delay (100);
      fastDigitalWrite(LED_BUILTIN, LOW);
		} else {
      
      checkButton(PSB_PAD_UP);

      checkButton(PSB_PAD_DOWN);
     
      checkButton(PSB_PAD_RIGHT);
     
      checkButton(PSB_PAD_LEFT);
     
      checkButton(PSB_SELECT);
     
      checkButton(PSB_START);
     
//			byte lx, ly;
//			psx.getLeftAnalog (lx, ly);
//			if (lx != slx || ly != sly) {
//				dumpAnalog ("Left", lx, ly);
//				slx = lx;
//				sly = ly;
//			}
//
//			byte rx, ry;
//			psx.getRightAnalog (rx, ry);
//			if (rx != srx || ry != sry) {
//				dumpAnalog ("Right", rx, ry);
//				srx = rx;
//				sry = ry;
//			}
		}
	}

	
	delay (1000 / 60);
}
