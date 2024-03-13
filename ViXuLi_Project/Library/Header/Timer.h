#ifndef  TIMER_H
#define	 TIMER_H
#define F_CPU	16000000UL
#include <avr/interrupt.h>

static volatile uint64_t milliseconds = 0;

ISR(TIMER0_COMPA_vect) {
	milliseconds++;
}
uint64_t millis() {
	uint64_t ms;
	cli();  
	ms = milliseconds;
	sei(); 
	return ms;
}
void init_timer()
{
	TCCR0A |= (1<<WGM01);
	OCR0A = 124;	//0,1ms
	TIMSK0 |= (1<<OCIE0A);
	TCCR0B |= (1<<CS01);
	sei();
}


#endif
