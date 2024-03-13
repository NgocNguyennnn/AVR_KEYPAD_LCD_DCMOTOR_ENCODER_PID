#ifndef LCD_H
#define LCD_H
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#define SLA_W	0x27<<1
#define WRITE	0
#define READ	1

void i2c_start();
void i2c_write_cmd(unsigned char address);
void i2c_write_data(unsigned char data);
void i2c_stop();
void i2c_init();
void LCD_WRITE(unsigned char x);
void LCD_SEND(unsigned char x);
void LCD_WRITE_COMMAND(unsigned char x);
void LCD_WRITE_DATA(unsigned char x);
void LCD_STRING(char *c);
void LCD_CLEAR();
void LCD_HOME();
void LCD_INIT();
void LCD_Cursor_Shift_Increa(uint8_t number);
void LCD_Cursor_Shift_Decrea(uint8_t number);

#endif
