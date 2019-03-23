/*
 * wspr bake
 *
 * Copyright (C) 2019 Markus Bühler, DK9MBS <m_buehler@t-online.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "si5351.h"
#include "Wire.h"

#define QRG 47420000ULL  // qrg for mw band
#define OFFSET 10000ULL //wspr band 0-200Hz
#define SYMBOL0 219ULL 
#define SYMBOL1 73ULL  //1,46 Hz over center qrg
#define SYMBOL2 73ULL  //1,46 Hz under center qrg
#define SYMBOL3 219ULL
#define DELAY 683ULL    //Duration for one symbol
#define TXLED 10
#define INITLED 11
#define STARTPIN 12
Si5351 si5351;

// DK9MBS JO52 10 generated with wsprcode.exe unter wine (windows emulator)

byte symbols[162] = { 
1,3,2,0,2,2,0,2,1,2,0,2,1,3,3,0,2,0,3,2,2,1,0,3,1,1,3,2,0,2,\
0,2,2,2,1,2,0,1,0,3,2,0,2,2,2,0,3,0,1,1,0,0,1,3,0,3,0,2,0,1,\
1,0,1,0,2,2,2,1,1,2,3,0,1,0,3,0,1,2,2,1,2,0,3,0,3,1,2,0,2,3,\
1,2,1,2,1,2,2,0,3,2,0,0,0,0,3,0,2,1,2,0,1,3,3,0,1,1,0,0,1,3,\
2,1,2,0,2,3,3,1,0,2,0,0,0,3,2,1,2,2,3,1,0,2,2,2,0,2,0,3,1,2,\
3,0,3,3,2,2,2,3,3,2,0,0                   
};

const unsigned long f0 = QRG + OFFSET - SYMBOL0;  
const unsigned long f1 = QRG + OFFSET - SYMBOL1;
const unsigned long f2 = QRG + OFFSET + SYMBOL2;
const unsigned long f3 = QRG + OFFSET + SYMBOL3;
unsigned long qrg[4] = {f0,f1,f2,f3};           

void setup()
{
  bool i2c_found;
  
  pinMode(STARTPIN, INPUT_PULLUP); //startbutton => set to ground for start
  pinMode(INITLED, OUTPUT); //status
  pinMode(TXLED, OUTPUT); //send control
  digitalWrite(INITLED,LOW);
  digitalWrite(TXLED,LOW);
  
  Serial.begin(57600);
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    Serial.println("Device not found on I2C bus!");
  }

  // Set CLK2 to output 476Khz
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK2, 0);

  si5351.update_status();
  delay(1000);
  digitalWrite(INITLED,HIGH);
  
}

void wsprStart() {

  Serial.println ("Txing wspr ...");
  digitalWrite(TXLED,HIGH);
  si5351.output_enable(SI5351_CLK2, 1);
  for(byte x=0;x<162;x++) {
    unsigned long long f=qrg[symbols[x]];  
    si5351.set_freq(f, SI5351_CLK2);
    Serial.print("Set freqency => ");Serial.print(x);Serial.println("");
    delay(DELAY);
  }

  si5351.output_enable(SI5351_CLK2, 0);
  digitalWrite(TXLED,LOW);
  Serial.println("Ready!");

}


void loop() {

  if(!digitalRead(STARTPIN)) { 
    wsprStart();
  }

}
