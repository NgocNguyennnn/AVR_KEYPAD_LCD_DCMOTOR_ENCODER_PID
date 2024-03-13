#ifndef ENCODER_PID_H
#define ENCODER_PID_H
#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include "Timer.h"
#include "Motor.h"
#include "LCD.h"
#include <string.h>

static uint64_t mil = 0;
static volatile uint16_t number = 0;
static uint32_t rpm = 0;
typedef struct PID{
	double kp;
	double ki;
	double kd;
	double Setpoint;
	double previousError;
	double integral;
	direct_t direct;
	uint16_t output;
	unsigned long SampleTime;
	unsigned long lastTime;
}PID_t;
PID_t Micro;
(Micro).previousError = 0;
(Micro).integral = 0;
double Setpoint(uint8_t *s)
{
	uint8_t count = strlen(s);
	if(count == 0) return (double)0;
	if(s[0] == '*')
	{
		if(count ==1) return (double)0;
		else{
			uint8_t rec[count];
			for(int i = 0;i<count;i++)
			{
				rec[i] = s[i+1];
			}
			return -((double)atoi(rec));
		}
	}
	else{
		return (double)atoi(s);
	}
}
double Set_K(uint8_t *s)
{
	uint8_t count = strlen(s);
	if(count == 0) return (double)0;
	if(s[1] == '*')
	{
		if(count == 2)
		{
			return (double)(s[0] - '0');
		}
		else{
			return (double)(((s[0]-'0')*10 + (s[2]-'0')))/10;
		}
	}
	else
	{
		return (double)atoi(s);
	}
}
ISR(INT1_vect)
{
	number++;
}
uint16_t pulse_encoder()
{
	uint16_t rotary;
	cli();
	rotary = number;
	number = 0;
	sei();
	return rotary;
}
uint32_t speed_encoder()
{
	if(millis()-mil >= 1000){
		mil = millis();
		rpm = (pulse_encoder()*60)/30;
		return rpm;
	}
	return rpm;
}
void PID_Init(PID_t *current,double kp,double kd,double ki,double Setpoint){
	if(Setpoint < 0)
	{
		Setpoint = -Setpoint;
		current->direct = counter_clockwise;
	}
	else if(Setpoint >= 0)
	{
		current->direct = clockwise;
	}

	current->kp = kp;
	current->ki = ki*0.1;
	current->kd = kd/0.1;
	current->Setpoint = Setpoint;
	//current->previousError=0;
	//current->integral=0;
	current->SampleTime = 100;
	current->lastTime = millis() - current->SampleTime;
}
void outputLimit(PID_t *current,int min,int max){
	if(current->output <min){
		current->output = min;
	}
	else if(current->output > max){
		current->output = max;
	}
}
void PID_Controller(PID_t *current,uint32_t speed)
{
	double derivative;
	unsigned long now;
	unsigned long timeChange;
	double error;
	double proportional;
	now = millis();
	timeChange = (now - current->lastTime);
	if(timeChange >= current->SampleTime){
		error = (double)(current->Setpoint - speed);
		if(error < 0)
		{
			error = -error;
		}
		proportional = current->kp *error;
		current->integral += current->ki *error;
		if(current->integral > 31000){
			current->integral = 31000;
		}
		else if(current->integral < 0){
			current->integral = 0;
		}
		derivative = current->kd *(error - current->previousError);
		current->output = (int) (proportional + current->integral - derivative);
		current->previousError = error;
		outputLimit(current,0,31999);
		switch(current->direct){
			case stop:
			exchang_para(0,stop);
			break;
			case clockwise:
			exchang_para(current->output,clockwise);
			break;
			case counter_clockwise:
			exchang_para(31999-current->output,counter_clockwise);
			break;
		}
	}
}
void encoder_init()
{
	EIMSK = 0x02;
	EICRA = 0x08;
	sei();
}

#endif
