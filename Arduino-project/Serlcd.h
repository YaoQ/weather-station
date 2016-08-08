#include <SoftwareSerial.h>

#define txPin 14

SoftwareSerial LCD = SoftwareSerial(0, txPin);

// since the LCD does not send data back to the Arduino, we should only define the txPin
const int LCDdelay=10;  // conservative, 2 actually works
 
// wbp: goto with row & column
void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}
void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
   delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}
 
void lcd_init()
{
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  backlightOn() ;
  clearLCD();
  lcdPosition(0,0);
  //LCD.print("Hello world from LinkSprite!");
}
