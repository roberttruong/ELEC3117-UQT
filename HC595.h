//Defines
#include <avr/io.h>
/***************************************

Configure Connections

****************************************/

#define HC595_PORT   PORTD
#define HC595_DDR    DDRD

#define HC595_DS_POS PD5      //Data pin (DS) pin location

#define HC595_SH_CP_POS PD6      //Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS PD7      //Store Clock (ST_CP) pin location


//Initialize HC595 System

extern void HC595Init();

//Sends a clock pulse on SH_CP line
extern void HC595Pulse();

//Sends a clock pulse on ST_CP line
extern void HC595Latch();

/*

Main High level function to write a single byte to
Output shift register 74HC595. 

Arguments:
   single byte to write to the 74HC595 IC

Returns:
   NONE

Description:
   The byte is serially transfered to 74HC595
   and then latched. The byte is then available on
   output line Q0 to Q7 of the HC595 IC.

*/
extern void HC595Write(uint8_t data);
