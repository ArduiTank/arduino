/*
  DMX_Master.ino - Example code for using the Conceptinetics DMX library
  Copyright (c) 2013 W.A. van der Meeren <danny@illogic.nl>.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <Conceptinetics.h>

#define intensity 9
#define red       8
#define green     7
#define blue      6
#define white     5

int Val_Int = 0;
int Val_Red = 0;
int Val_Green = 0;
int Val_Blue = 0;
int Val_White = 0;

//
// CTC-DRA-13-1 ISOLATED DMX-RDM SHIELD JUMPER INSTRUCTIONS
//
// If you are using the above mentioned shield you should 
// place the RXEN jumper towards pin number 2, this allows the
// master controller to put to iso shield into transmit 
// (DMX Master) mode 
//
//
// The !EN Jumper should be either placed in the G (GROUND) 
// position to enable the shield circuitry 
//   OR
// if one of the pins is selected the selected pin should be
// set to OUTPUT mode and set to LOGIC LOW in order for the 
// shield to work
//


//
// The master will control 100 Channels (1-100)
// 
// depending on the ammount of memory you have free you can choose
// to enlarge or schrink the ammount of channels (minimum is 1)
//
#define DMX_MASTER_CHANNELS   10 

//
// Pin number to change read or write mode on the shield
//
#define RXEN_PIN                2


// Configure a DMX master controller, the master controller
// will use the RXEN_PIN to control its write operation 
// on the bus
DMX_Master        dmx_master ( DMX_MASTER_CHANNELS, RXEN_PIN );


// the setup routine runs once when you press reset:
void setup() {             
  
  // Enable DMX master interface and start transmitting
  dmx_master.enable ();  
  
  // Set channel 1 - 50 @ 50%
  dmx_master.setChannelRange ( 1, 25, 127 );

  
  pinMode(intensity, INPUT);
  pinMode(red, INPUT);
  pinMode(green, INPUT);
  pinMode(blue, INPUT);
  pinMode(white, INPUT);
}

// the loop routine runs over and over again forever:
void loop() 
{
  static int dimmer_val;
  
  // Keep fading channel 1 in from 0 to 100%
  // Intensity
  /*if (digitalRead(intensity) == 0) {
    if (Val_Int > 100) {
      Val_Int = 0;
    }
    Val_Int += 1;
    dmx_master.setChannelValue (1, Val_Int);
  }*/
  dmx_master.setChannelValue (1, 100);
  // Red
  if (digitalRead(red) == 0) {
    if (Val_Red > 99) {
      Val_Red = 0;
    }
    Val_Red += 1;
    dmx_master.setChannelValue (2, Val_Red);
  }
  // Green
  if (digitalRead(green) == 0) {
    if (Val_Green > 99) {
      Val_Green = 0;
    }
    Val_Green += 1;
    dmx_master.setChannelValue (3, Val_Green);
  }
  // Blue
  if (digitalRead(blue) == 0) {
    if (Val_Blue > 99) {
      Val_Blue = 0;
    }
    Val_Blue += 1;
    dmx_master.setChannelValue (4, Val_Blue);
  }
  // White
  if (digitalRead(white) == 0) {
    if (Val_White > 99) {
      Val_White = 0;
    }
    Val_White += 1;
    dmx_master.setChannelValue (5, Val_White);
  }
  delay(200);
}
