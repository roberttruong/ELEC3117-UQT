#include <avr/io.h>


void spi_init(void){
	//Set SS SCLK and MOSI as output
	DDRB |= (1<<DDB2)|(1<<DDB3)|(1<<DDB5);
	
	//Set the SPI Control Register
	SPCR = (1<<SPE)|(1<<MSTR);
	//volatile char IOReg;
	//IOReg   = SPSR;  // clear SPIF bit in SPSR
	//IOReg   = SPDR;
	//Disable SS
	PORTB |= (1<<PORTB2); 
}

unsigned char spi_data(unsigned char data){
	//SS enable (Active LOW)
	PORTB &= ~(1<<PORTB2); 
	//Load data
	SPDR = data;
	while(!(SPSR&(1<<SPIF))){
		
	}
	return(SPDR);
	//SS Disable
	PORTB |= (1<<PORTB2); 
}