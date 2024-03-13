#ifndef KEYPAD_H
#define KEYPAD_H
#include "avr/io.h"
#include "stdint.h"
#include "ENCODER_PID.h"
#include "Timer.h"
#include "Display.h"

#define KEYPAD_ROW	4
#define KEYPAD_COL	3

double PID_Para[4] = {0,0,0,0};
uint8_t Keypad_RowPin[KEYPAD_ROW] = {PD4,PD5,PD6,PD7};
uint8_t Keypad_ColPin[KEYPAD_COL] = {PD0,PD1,PD2};
	
static uint8_t	key_deboune;
static uint8_t	debouncing = 0;
static uint64_t	t_debounce;
static uint8_t key_current;
	
const uint8_t Key_code[KEYPAD_ROW][KEYPAD_COL] =
{
	{'7','8','9'},
	{'4','5','6'},
	{'1','2','3'},
	{'*','0','#'},
};

void Keypad_Init()
{
	DDRD |= (1<<4) | (1<<5) | (1<<6) | (1<<7);
	PORTD |= (1<<PD0) | (1<<PD1) | (1<<PD2);
}
void Keypad_Select_Row(uint8_t Row)
{
	PORTD &= ~(1<<Keypad_RowPin[Row]);
}
void Keypad_Unselect_Row()
{
	for(uint8_t row=0; row<KEYPAD_ROW; row++)
	{
		PORTD |= (1<<Keypad_RowPin[row]);
	}
}
uint8_t Keypad_Getkey()
{
	for(uint8_t row = 0;row<KEYPAD_ROW;row++)
	{
		Keypad_Unselect_Row();
		Keypad_Select_Row(row);
		for(uint8_t col = 0; col<KEYPAD_COL;col++)
		{
			if((PIND & (1<<Keypad_ColPin[col]))>>Keypad_ColPin[col] == 0)
			{
				return Key_code[row][col];
			}
		}
	}
	return 0;
}
void Keypad_Filter()
{
	uint8_t key = Keypad_Getkey();
	if(key != key_deboune)
	{
		t_debounce = millis();
		debouncing = 1;
		key_deboune = key;
	}
	if(debouncing && (millis() - t_debounce >= 15))
	{
		key_current = key_deboune;
		debouncing = 0;
	}
}
void Keypad_callback()
{
	uint32_t speed = speed_encoder();
	if(para_flag == 1)
	{
		para_flag = 0;
		uint8_t *token;
		if(strcmp(para,"|||") == 0){
			return ;
		}
		
		token = strtok(para, "|");
		PID_Para[0] = Setpoint(token);
		uint8_t i = 1;
		
		while (token != NULL)
		{
			token = strtok(NULL, "|");
			if (token != NULL)
			{
				PID_Para[i++] = Set_K(token);
			}
		}
		PID_Init(&Micro,PID_Para[1],PID_Para[2],PID_Para[3],PID_Para[0]);
	}
	if(status == screen_encoder)
	{
		LCD_HOME();
		LCD_Cursor_Shift_Increa(7);
		LCD_print("%03d rpm",speed);
		PID_Controller(&Micro,speed);
		if(Micro.direct == clockwise)
		{
			LCD_WRITE_COMMAND(0xC0);
			LCD_print("Clockwise");
		}
		else if(Micro.direct == counter_clockwise)
		{
			LCD_WRITE_COMMAND(0xC0);
			LCD_print("CounterClockwise");
		}
	}
}

void Keypad_Handle()
{
	Keypad_Filter();
	if(key_current == '#')
	{
		switch(status)
		{
			case screen_default:
			status = screen_input;
			break;
			case screen_input:
			para[para_length++] = '|';
			error_kp = para_length;
			status = screen_kp;
			break;
			case screen_kp:
			para[para_length++] = '|';
			error_kd = para_length;
			status = screen_kd;
			break;
			case screen_kd:
			para[para_length++] = '|';
			error_ki = para_length;
			status = screen_ki;
			break;
			case screen_ki:
			para[para_length] = '\0';
			para_flag = 1;
			status = screen_encoder;
			break;
			case screen_encoder:
			para_length = 0;
			status = screen_input;
			break;
			case screen_error:
			status = current;
			break;
		}
		Display_handle();
		key_current = 0;
	}
	if(key_current != 0 && key_current != '#' && para_flag == 0)
	{
		uint8_t key_sta = key_current;
		Keypad_Filter();
		if(key_current == 0){
		switch(status)
		{
			case screen_input:
					current = status;
					ki_screen = 0xC3;
					LCD_WRITE_COMMAND(0x0D);
					LCD_WRITE_COMMAND(input_screen++);
					para[para_length++] = key_sta;
					if(key_sta == '*' && para_length >1){
						Display_Error(&input_screen,0xC2,1,&current,&para_length);
					}
					else{
						if(para[0] == '*')
						{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&input_screen,0xC2,4,&current,&para_length);
						}
						else
						{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&input_screen,0xC2,3,&current,&para_length);
						}
					}
			break;
			case screen_kp:
					current = status;
					input_screen = 0xC2;
					LCD_WRITE_COMMAND(kp_screen++);
					para[para_length++] = key_sta;
					if(para[error_kp] == '*')
					{
						Display_Error(&kp_screen,0xC3,0,&current,&para_length);
					}
					else
					{
						if(para[error_kp+1] == '*')
						{
							if((para_length - error_kp > 3) ||(para[error_kp+2] == '*'))
							{
								Display_Error(&kp_screen,0xC3,0,&current,&para_length);
							}
							else
							{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&kp_screen,0xC3,3,&current,&para_length);
							}
						}
						else
						{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&kp_screen,0xC3,2,&current,&para_length);
						}
					}
			break;
			case screen_kd:
					current = status;
					kp_screen = 0xC3;
					LCD_WRITE_COMMAND(kd_screen++);
					para[para_length++] = key_sta;
					if(para[error_kd] == '*')
					{
						Display_Error(&kd_screen,0xC3,0,&current,&para_length);
					}
					else
					{
						if(para[error_kd+1] == '*')
						{
							if((para_length - error_kd > 3) ||(para[error_kd+2] == '*'))
							{
								Display_Error(&kd_screen,0xC3,0,&current,&para_length);
							}
							else
							{
								LCD_WRITE_DATA(key_sta);
								Display_Error(&kd_screen,0xC3,3,&current,&para_length);
							}
						}
						else
						{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&kd_screen,0xC3,2,&current,&para_length);
						}
					}
			break;
			case screen_ki:
					current = status;
					kd_screen = 0xC3;
					LCD_WRITE_COMMAND(ki_screen++);
					para[para_length++] = key_sta;
					if(para[error_ki] == '*')
					{
						Display_Error(&ki_screen,0xC3,0,&current,&para_length);
					}
					else
					{
						if(para[error_ki+1] == '*')
						{
							if((para_length - error_ki > 3) ||(para[error_ki+2] == '*'))
							{
								Display_Error(&ki_screen,0xC3,0,&current,&para_length);
							}
							else
							{
								LCD_WRITE_DATA(key_sta);
								Display_Error(&ki_screen,0xC3,3,&current,&para_length);
							}
						}
						else
						{
							LCD_WRITE_DATA(key_sta);
							Display_Error(&ki_screen,0xC3,2,&current,&para_length);
						}
					}
			break;
		}
		}
	}
	Keypad_callback();
}

#endif

