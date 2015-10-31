// ELEC3117 
// Robert Truong
// Nabi Genc


#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

// Define Variables
#define MAX_CURRENT 2000
#define INT_TO_ASCII 48
#define VOLTAGE_CHANNEL 1
#define RIPPLE_CHANNEL 0 
#define VOLTAGE_SCALE 0.48828125
#define VOLTAGE_DIVIDER 2
#define CURRENT_VOLTAGE_THRESHOLD 350
#define VOLTAGE_HIGH 550
#define VOLTAGE_LOW 450
#define RIPPLE_THRESHOLD 250
#define RIPPLE_SCALE 0.48828125
#define RIPPLE_REVERSE 5.44
#define RIPPLE_OFFSET 45
#define PADDING 5
#define TEST_DELAY 300


// Define LCD Pins
#define D4 eS_PORTD2
#define D5 eS_PORTD3
#define D6 eS_PORTD4
#define D7 eS_PORTB0
#define RS eS_PORTD0
#define EN eS_PORTD1

volatile int timer_flag;

//Define Libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "SPI.h"
#include "HC595.h"
#include "pot.h"
#include "adc.h"
#include "timer1.h"



int main(void)
{	/////////////////// SETUP //////////////////////
	//Setting up output for LCD Pins
	DDRD |= (1<<DDD0)|(1<<DDD1)|(1<<DDD2)|(1<<DDD3)|(1<<DDD4);
	DDRB |= (1<<DDB0);
	
	// LCD Initialisation
	Lcd4_Init();
	
	// SPI Initialisation
	spi_init();
	
	// Shift register Initialisation
	// Pins defined in header file. 
	HC595Init();
	
	// ADC Initialisation
	adc_init();
	
	// Timer Initialisation
	timer_init();
	
	// Define Inputs
	// ADC input
	DDRC &= ~(1<<DDC0);
	DDRC &= ~(1<<DDC1);
	// Push Button
	DDRB &= ~(1<<DDB1);
	
	////////////////////////////////////////////////
	
	///////////////// Variables ////////////////////
	int numberPoints = ceil(MAX_CURRENT/39)+1; //Number of points based on current test 
	int k,n,o,b;
	int flagVoltage,flagRipple,temp,reset_flag;
	char testing[8];
	char amps[5];
	char volts[8];
	int maximumCurrent,maximumVoltage,maximumRipple;
	uint16_t dataRipple[numberPoints];
	uint16_t dataVoltage[numberPoints];
	uint16_t m;
	uint8_t led_pattern[9]={
		0b00000001,
		0b00000011,
		0b00000111,
		0b00001111,
		0b00011111,
		0b00111111,
		0b01111111,
		0b11111111,
		0b00000000,
	};
	
	////////////////////////////////////////////////
	
	// Startup Code
	pot_write(0);
	HC595Write(led_pattern[8]);
	Lcd4_Clear();
	
	// LOOP
	while(1)
	{	/////////////// Startup ////////////
		// Initial text on LCD 
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,2);
		Lcd4_Write_String("USB Quality");
		Lcd4_Set_Cursor(2,5);
		Lcd4_Write_String("Tester");
		_delay_ms(1000);
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,1);
		Lcd4_Write_String("Press to begin");
		Lcd4_Set_Cursor(2,6);
		Lcd4_Write_String("test");
		Lcd4_Set_Cursor(2,15);
		Lcd4_Write_Char(0b01111110); //Arrow
		
		
		
		while((PINB & (1<<PB1))==0){ //Wait for button to be pressed
		}
		
		// Formating LCD Text
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("Current: ");
		Lcd4_Set_Cursor(1,14);
		Lcd4_Write_Char('A');
		
		Lcd4_Set_Cursor(2,0);
		//Lcd4_Write_String("Voltage: "); REMOVE LATER
		Lcd4_Set_Cursor(2,14);
		Lcd4_Write_Char('V');
		
		///////////////////////////////////////
	
		/////////////// Test //////////////////
				
		for(int i=0;i<numberPoints;i++){
			// Set current
			pot_write(i);			
			k = floor((i+1)*3.9); //convert to centiamps
			
			// Formating String for LCD
			amps[0]=floor(k/100)+INT_TO_ASCII;
			amps[1]='.';
			amps[2]=floor((k%100)/10)+INT_TO_ASCII;
			amps[3]=k%10+INT_TO_ASCII;
			amps[4]='\0';
			
			// Writing string
			Lcd4_Set_Cursor(1,9);
			Lcd4_Write_String(amps);
			
			// Voltage level
			// Implement raw data collection
			dataVoltage[i]= adc_read(VOLTAGE_CHANNEL);
			
			// Formating string for LCD
			m= floor((dataVoltage[i]+1)*VOLTAGE_SCALE*VOLTAGE_DIVIDER);
			volts[0]=floor(m/100)+INT_TO_ASCII;
			volts[1]='.';
			volts[2]=floor((m%100)/10)+INT_TO_ASCII;
			volts[3]=m%10+INT_TO_ASCII;
			volts[4]='\0';
			
			// Display Voltage on LCD
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(volts);
			
			// Ripple magnitude
			// Implement raw data collection
			
			b= adc_read(RIPPLE_CHANNEL);
			if (b>RIPPLE_OFFSET){
				dataRipple[i]=b-RIPPLE_OFFSET;
			} else {
				dataRipple[i]=0;
			}
			
			//testing REMOVE IN FINAL
			m= floor((dataRipple[i]+1)*RIPPLE_SCALE);
			testing[0]=floor(m/100)+INT_TO_ASCII;
			testing[1]='.';
			testing[2]=floor((m%100)/10)+INT_TO_ASCII;
			testing[3]=m%10+INT_TO_ASCII;
			testing[4]='\0';
			//
			
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String(testing);
			// Blick LED ladder while testing
			if(i%2==0){
				HC595Write(led_pattern[8]);
			} else {
				HC595Write(led_pattern[7]);
			}
			
			_delay_ms(TEST_DELAY);			
		}
		pot_write(0);
		HC595Write(led_pattern[8]);
		
		/////////// Data analysis //////////////
		
		// Max Current
		maximumCurrent = MAX_CURRENT/10;
		o=numberPoints;
		for (int i=numberPoints-1;i>=0;i--){
			if (dataVoltage[i]<	CURRENT_VOLTAGE_THRESHOLD){
				maximumCurrent = floor((i+1)*3.9);
				o=i;
			}
		}
		
		// Voltage levels
		maximumVoltage= 0;
		flagVoltage = 0;
		for(int i=0;i<o-PADDING;i++){
			temp = floor((dataVoltage[i]+1)*VOLTAGE_SCALE*VOLTAGE_DIVIDER);
			if(temp>VOLTAGE_HIGH){
				flagVoltage = 1;
			}
			if (temp>maximumVoltage){
				maximumVoltage = temp;
			}
			
		}
		maximumRipple = 0;
		// Ripple Test
		flagRipple = 0;
		for(int i=0;i<o-PADDING;i++){
			temp = floor(dataRipple[i]*RIPPLE_SCALE);
			if(temp>RIPPLE_THRESHOLD){
				flagRipple = 1;
			}
			if (temp>maximumRipple){
				maximumRipple = temp;
			}
		}
		
		maximumRipple = maximumRipple/RIPPLE_REVERSE;
		
			
		////////////////////////////////////////	

		/////////// Display Results ////////////
		Lcd4_Clear();
		
		// Displaying max current
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("MaxCurrent: ");
		amps[0]=floor(maximumCurrent/100)+INT_TO_ASCII;
		amps[1]='.';
		amps[2]=floor((maximumCurrent%100)/10)+INT_TO_ASCII;
		amps[3]='\0';
		Lcd4_Write_String(amps);
		Lcd4_Write_Char('A');
		Lcd4_Set_Cursor(2,0);
		
		// Verbose messages based on signal quality
		if (flagRipple|flagVoltage){
			Lcd4_Write_String("Danger");
		} else{	
			Lcd4_Write_String("Good");
		}
		
		//LED ladder based on max current and signal quality
		n = floor(maximumCurrent*8/200);
		if(flagVoltage||flagRipple){
			HC595Write(led_pattern[8]);
		} else {
			HC595Write(led_pattern[n-1]);
		}
	
		////////////////////////////////////////
		
		/////////// More & Reset ///////////////
		Lcd4_Set_Cursor(2,11);
		Lcd4_Write_String("Next");
		Lcd4_Write_Char(0b01111110);
		
		// wait for button press
		while((PINB & (1<<PB1))==0){
		}
		
		//reset timer. and flag
		timer_flag = 0;
		reset_flag = 0;
		TCNT1 = 0;
		
		// test how long button is pressed. if held longer than 1 sec, break and reset
		while((PINB & (1<<PB1))==2){
			if(timer_flag == 1){
				reset_flag = 1;
				break;
			}
		}
	
		if(reset_flag)
			continue;
			
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("MaxVoltage: ");
		testing[0]=floor(maximumVoltage/100)+INT_TO_ASCII;
		testing[1]='.';
		testing[2]=floor((maximumVoltage%100)/10)+INT_TO_ASCII;
		testing[3]='\0';
		Lcd4_Write_String(testing);
		Lcd4_Write_Char('V');
		Lcd4_Set_Cursor(2,0);	

		if(flagVoltage){
			HC595Write(led_pattern[8]);
		} else {
			HC595Write(led_pattern[7]);
		}
		
		/////////// More & Reset ///////////////
		Lcd4_Set_Cursor(2,11);
		Lcd4_Write_String("Next");
		Lcd4_Write_Char(0b01111110);
		
		// wait for button press
		while((PINB & (1<<PB1))==0){
		}
		
		//reset timer. and flag
		timer_flag = 0;
		reset_flag = 0;
		TCNT1 = 0;
		
		// test how long button is pressed. if held longer than 1 sec, break and reset
		while((PINB & (1<<PB1))==2){
			if(timer_flag == 1){
				reset_flag = 1;
				break;
			}
		}
		
		if(reset_flag)
		continue;
		
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("MaxRipple: ");
		testing[0]=floor(maximumRipple/100)+INT_TO_ASCII;
		testing[1]='.';
		testing[2]=floor((maximumRipple%100)/10)+INT_TO_ASCII;
		testing[3]=maximumRipple%10+INT_TO_ASCII;
		testing[4]='\0';
		Lcd4_Write_String(testing);
		Lcd4_Write_Char('V');
		Lcd4_Set_Cursor(2,0);

		if(flagRipple){
			HC595Write(led_pattern[8]);
			} else {
			HC595Write(led_pattern[7]);
		}
		
		/////////// More & Reset ///////////////
		Lcd4_Set_Cursor(2,10);
		Lcd4_Write_String("Reset");
		Lcd4_Write_Char(0b01111110);
		
		// wait for button press
		while((PINB & (1<<PB1))==0){
		}
		
		///////////////////////////////////////
	}
}
