#ifndef Motor_H
#define Motor_H
#include <avr/io.h>
#define F_CPU 16000000UL
#include <stdlib.h>
#include <stdint.h>

typedef enum{
	clockwise = 0,
	counter_clockwise = 1,
	stop,
}direct_t;
void exchang_para(uint16_t output, direct_t motor);
void init_motor();


#endif
