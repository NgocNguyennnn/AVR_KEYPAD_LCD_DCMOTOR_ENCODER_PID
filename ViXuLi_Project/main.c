/*
 * ViXuLi_Project.c
 *
 * Created: 12/19/2023 4:13:40 PM
 * Author : Dell Precision 7510
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <stdlib.h>
#include <stdint.h>
#include "avr/interrupt.h"
#include "LCD.h"
#include "Display.h"
#include "Motor.h"
#include "ENCODER_PID.h"
#include "Keypad.h"
#include "Timer.h"
#include <util/delay.h>


int main(void)
{
	init_timer();
	encoder_init();
	init_motor();
	Keypad_Init();
	LCD_INIT();
	Display_Init();
	Display_handle();
	while (1)
	{
			Keypad_Handle();
	}
	
}

