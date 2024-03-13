#include "LCD.h"
#include <stdio.h>

static unsigned char lcd = 0x00;

void i2c_start()
{
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		//--- Start Condition as per Datasheet
	while(!(TWCR & (1<<TWINT)));				//--- Wait till start condition is transmitted to Slave
}
void i2c_write_cmd(unsigned char address)
{
	TWDR=address;								//--- SLA Address and write instruction
	TWCR=(1<<TWINT)|(1<<TWEN);					//--- Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));				//--- Wait till complete TWDR byte transmitted to Slave
}
void i2c_write_data(unsigned char data)
{
	TWDR=data;									//--- Put data in TWDR
	TWCR=(1<<TWINT)|(1<<TWEN);					//--- Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT)));				//--- Wait till complete TWDR byte transmitted to Slave
}
void i2c_stop()
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		//--- Stop Condition as per Datasheet
}
void i2c_init(){
	TWCR &= ~(1<<TWEN);
	TWSR = 0x00;		//Prescaler set to 1
	TWBR = 0x62;		//	Baud rate is set by calculating
	TWCR = (1<<TWEN);	//Enable I2C
	_delay_us(10);
}
void LCD_WRITE(unsigned char x)
{
	i2c_start();							//--- Start Condition
	i2c_write_cmd(SLA_W| WRITE);	//--- SLA+W is Send 0x40
	i2c_write_data(x);						//--- Data to Slave Device
	i2c_stop();								//--- Stop Condition
}
void LCD_SEND(unsigned char x)
{
	unsigned char temp = 0x00;					//--- temp variable for data operation
	
	lcd &= 0x0F;								//--- Masking last four bit to prevent the RS, RW, EN, Backlight
	temp = (x & 0xF0);							//--- Masking higher 4-Bit of Data and send it LCD
	lcd |= temp;								//--- 4-Bit Data and LCD control Pin
	lcd |= (0x04);								//--- Latching Data to LCD EN = 1
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(1);								//--- 1us Delay
	lcd &= ~(0x04);								//--- Latching Complete
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(5);								//--- 5us Delay to Complete Latching
	
	
	temp = ((x & 0x0F)<<4);						//--- Masking Lower 4-Bit of Data and send it LCD
	lcd &= 0x0F;								//--- Masking last four bit to prevent the RS, RW, EN, Backlight
	lcd |= temp;								//--- 4-Bit Data and LCD control Pin
	lcd |= (0x04);								//--- Latching Data to LCD EN = 1
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(1);								//--- 1us Delay
	lcd &= ~(0x04);								//--- Latching Complete
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	_delay_us(5);								//--- 5us Delay to Complete Latching
	
}
void LCD_WRITE_COMMAND(unsigned char x)
{
	lcd = 0x08;									//--- Enable Backlight Pin
	//lcd &= ~(0x01);								//--- Select Command Register By RS = 0
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	LCD_SEND(x);						//--- Function to Write 4-bit data to LCD
}
void LCD_WRITE_DATA(unsigned char x)
{
	lcd |= 0x09;								//--- Enable Backlight Pin & Select Data Register By RS = 1
	LCD_WRITE(lcd);							//--- Send Data From PCF8574 to LCD PORT
	LCD_SEND(x);						//--- Function to Write 4-bit data to LCD
}
void LCD_STRING(char *c)
{
	/*
	while (*c != '\0')
	{
		LCD_WRITE_DATA(*c++);
		//_delay_ms(50);
	}
	*/
	for(int i=0; c[i] != 0;i++)
	{
		LCD_WRITE_DATA(c[i]);
	}
}
void LCD_CLEAR()
{
	LCD_WRITE_COMMAND(0x01);
}
void LCD_HOME()
{
	LCD_WRITE_COMMAND(0x02);
}
void LCD_INIT()
{
	i2c_init();
	LCD_HOME();								// Return Home
	LCD_WRITE_COMMAND(0x28);				//Function Set
	LCD_WRITE_COMMAND(0x0C);				//Display Set On/Off
	LCD_WRITE_COMMAND(0x06);				//Entry Mode
	LCD_CLEAR(0x01);						//Clear
}
void LCD_Cursor_Shift_Increa(uint8_t number)
{
	for(int i=0; i<number-1;i++)
	{
		LCD_WRITE_COMMAND(0x14);
	}
}
void LCD_Cursor_Shift_Decrea(uint8_t number)
{
	for(int i=0; i<number-1;i++)
	{
		LCD_WRITE_COMMAND(0x10);
	}
}
void LCD_print(const char *str, ...) {
	char stringArr[30];

	va_list arg;
	va_start(arg, str);
	vsnprintf(stringArr, sizeof(stringArr), str, arg);
	va_end(arg);

	LCD_STRING(stringArr);
}
void LCD_printFloat(float value, uint8_t decimalPlaces) {
	char buffer[15];  // Adjust the size based on your requirements

	// Format the float value with the specified number of decimal places
	int charactersWritten = snprintf(buffer, sizeof(buffer), "%.*f", decimalPlaces, value);

	// Check for errors or buffer overflow
	if (charactersWritten < 0 || charactersWritten >= sizeof(buffer)) {
		// Handle error, buffer overflow, etc.
		return;
	}

	LCD_STRING(buffer);
	_delay_ms(100);
}




