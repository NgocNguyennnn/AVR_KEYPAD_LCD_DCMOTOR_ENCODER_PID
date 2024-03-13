#include "Motor.h"


void exchang_para(uint16_t output, direct_t motor)
{
	uint8_t exchange;
	if(motor == stop){
		output = 0;
		exchange = 0;
	}
	else if(motor == clockwise)
	{
		exchange = 0;
	}
	else{
		exchange = 1;
	}
	OCR1A = output;
	PORTC |= (exchange << PC0);
}
void init_motor(){
		TCCR1A=0; TCCR1B=0;
		DDRB |= (1 << PB1);
		DDRC |= (1 << PC0);
		TCCR1B |= (1 << CS10) | (1 << WGM12) |  (1 << WGM13);
		TCCR1A |= (1 << WGM11);
		TCCR1A |= (1 << COM1A1);
		ICR1 = 31999;
		OCR1A = 0;
}
