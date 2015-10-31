void timer_init(){
	// Set prescaler to 256 so F = 62500hz so overflow time approx 1 sec
	TCCR1B |= (1<<CS12);
	
	// Start counter
	TCNT1 = 0;
	
	// Enable interrupt overflow
	TIMSK1 = (1<<TOIE1);
	
	sei();
	
	timer_flag = 0;	
	
}
ISR(TIMER1_OVF_vect){
	timer_flag = 1;
}