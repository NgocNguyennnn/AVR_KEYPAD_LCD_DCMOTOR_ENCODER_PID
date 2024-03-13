#ifndef  DISPLAY_H
#define	 DISPLAY_H
#include "LCD.h"
#include "ENCODER_PID.h"
#include <stdint.h>
#define MAX_PARA 20

typedef enum
	{
	screen_error,
	screen_default,
	screen_input,
	screen_kp,
	screen_kd,
	screen_ki,
	screen_encoder,	
	}display_t;
display_t	status;
display_t	current;
uint16_t  input_screen = 0xC2;
uint16_t  kp_screen = 0xC3;
uint16_t  kd_screen = 0xC3;
uint16_t  ki_screen = 0xC3;
uint8_t error_kp;
uint8_t error_kd;
uint8_t error_ki;
uint8_t para_length = 0;
uint8_t para[MAX_PARA];
uint8_t para_flag = 0;

void Display_Init()
{
	status = screen_default;
}
void Clear_buff(uint8_t para_length,uint8_t error_)
{
	for(uint8_t i=error_; i<=para_length;i++)
	{
		para[i] = 'A';
	}
	return;
}
void Display_Error(uint16_t *var,uint16_t address,uint8_t length,display_t *current,uint8_t *para_length)
{
	if(*var >= address + length +1)
	{
		status = screen_error;
		LCD_CLEAR();
		LCD_HOME();
		LCD_STRING("ERROR: TOO ARG");
		*var = address;
		switch(*current)
		{
			case screen_input:
			PID_Init(&Micro,0,0,0,0);
			*current = screen_input;
			*para_length = 0;
			break;
			case screen_kp:
			*current = screen_kp;
			Clear_buff(*para_length,error_kp);
			*para_length = error_kp;
			break;
			case screen_kd:
			*current = screen_kd;
			Clear_buff(*para_length,error_kd);
			*para_length = error_kd;
			break;
			case screen_ki:
			*current = screen_ki;
			Clear_buff(*para_length,error_ki);
			*para_length = error_ki;
			break;
		}
	}
}
void Display_handle()
{
	switch(status)
	{
		case screen_default:
		LCD_CLEAR();
		LCD_HOME();
		LCD_Cursor_Shift_Increa(4);
		LCD_STRING("WELCOME TO");
		LCD_WRITE_COMMAND(0xC0);
		LCD_STRING("PROJECT GROUP 10");
		break;
		case screen_input:
		LCD_CLEAR();
		LCD_HOME();
		LCD_STRING("ENTER SPEED:");
		break;
		case screen_kp:
		LCD_CLEAR();
		LCD_HOME();
		LCD_Cursor_Shift_Increa(2);
		LCD_STRING("SET PARAMETERS");
		LCD_WRITE_COMMAND(0xC0);
		LCD_STRING("Kp=");
		break;
		case screen_kd:
		LCD_CLEAR();
		LCD_HOME();
		LCD_WRITE_COMMAND(0xC0);
		LCD_STRING("Kd=");
		break;
		case screen_ki:
		LCD_CLEAR();
		LCD_HOME();
		LCD_WRITE_COMMAND(0xC0);
		LCD_STRING("Ki=");
		break;
		case screen_encoder:
		LCD_CLEAR();
		LCD_HOME();
		LCD_print("Speed:");
		break;
	}
}

#endif
