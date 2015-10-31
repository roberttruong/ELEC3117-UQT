/*
 * adc.h
 *
 * Created: 29/09/2015 12:31:53 PM
 *  Author: Robert Truong
 */ 


void adc_init(){
	// use ARef
	// aka do nothing
	
	// Turn ADC on and set prescaler = 128
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}

uint16_t adc_read(uint8_t channel){
	channel &= 0b00000111; //for safety mask out only last 3 bits
	
	ADMUX &= ~(0b00000111); //write channel to admux
	ADMUX |= channel;
	
	ADCSRA |= 1<<ADSC; //start adc
	
	while(ADCSRA & (1<<ADSC)){
	}
	
	return ADC;
}