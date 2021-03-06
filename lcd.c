#ifndef lcd_C
#define lcd_C

#include <avr\io.h>
#include <stdio.h>
#include <util\delay.h>
#include "lcd.h"

void lcd_config(void)
{
	DDRB=0b11111100;
}

void lcd_init(void)
{
	lcd_config();
	_delay_ms(30);				// wait after power up

	lcd_nibble(0x03,0);			// send 03h 3 times to initialize

	lcd_en_tgl();
	_delay_ms(5);

	lcd_en_tgl();
	_delay_us(160);

	lcd_en_tgl();
	_delay_ms(5);

	lcd_nibble(0x02,0);			// enable 4 bit mode
	_delay_us(160);

	lcd_cmd(0x28);					//set 4-bit mode and 2 lines @ 5x7
	_delay_us(160);

	lcd_cmd(0x10);					//cursor move & shift left
	_delay_us(160);

	lcd_cmd(0x06);					//entry mode = increment
	_delay_us(160);

	lcd_cmd(0x0C);					//display on - cursor blink on
	_delay_us(160);

	lcd_cmd(0x01);					//clear display ram
	_delay_ms(2);
}

void lcd_en_tgl (void)
{
	PORTB|=(1<<EN);
	PORTB&=~(1<<EN);					//Enable pin
}

void lcd_cmd(unsigned char letter)
{
// Command Function
//The RS is set to 0 to signify this is a command
	unsigned char temp;             // temp Variable

	temp=letter;                    //move letter to temp
	temp=temp>>4;                   //shift temp to right by 4
	lcd_nibble(temp,0);             //send out higher nibble

	temp=letter;                    //move letter to temp
	temp=temp&0x0F;                 
	lcd_nibble(temp,0);             //send out lower nibble
	_delay_us(10);
}

void lcd_char(unsigned char letter)
{
//TData  Function
//The RS is set to 1 to signify this is a command
	unsigned char temp;             // temp Variable

	temp = letter;                    //move letter to temp
	temp = temp>>4;                   //shift temp to right by 4
	lcd_nibble(temp,1);             //send out higher nibble

	temp=letter;                    //move letter to temp
	temp=temp&0x0F;                 
	lcd_nibble(temp,1);             //send out lower nibble

	_delay_us(10);
}

void lcd_nibble(unsigned char nib,unsigned char reg_sel)
{
	if(reg_sel)
	PORTB |= (1<<RS);               	// Set RS Pin (defined in header file)
	else
	PORTB &= ~(1<<RS); 
	nib = (nib<<4);
	PORTB &= 0x0F;
	PORTB |= nib;
	_delay_us (5);
	lcd_en_tgl ();                   // Toggle E pin (defined in header file)
	_delay_us(400);
}

void lcd_clr_line(unsigned char line)
{
// clear line command.
    char tline;                             // The line to clear variable
    char x;                                 // loop variable

    if(line==1)                             // Set the variable value based on line
        tline=0x80;                         // 1 = Line 1
    if(line==2)
        tline=0xC0;                         // 2 = Line 2
	lcd_cmd(tline);                         // Send command to jump to beggining of line (1/2)
    _delay_us(160);

    for(x=0x00;x<0x14;x++)
	{                   					    // Loop through all 20 chars of line (even tho 16 are viewable)
        lcd_char(0x20);                     // Send Blank Character
        _delay_us(160);
    }

    lcd_cmd(tline);                         // Go back to beggining of line
    _delay_us(160);
}

void lcd_string(unsigned char *senpoint, unsigned char line)
{
    if(line==1)
        lcd_cmd(line1);
    if(line==2)
        lcd_cmd(line2);
    while(*senpoint != '\0')            // While we havent seen a \0 (esc) go on
	{
		lcd_char(*senpoint);            // Send 1st char to our char function
		senpoint++;                     // Send next
	}
}

lcd_position_char(unsigned char *senpoint,unsigned char line,unsigned char position)
{
	unsigned char pos;
	pos=position;
	lcd_cmd(0x03);						//return cursor to home position
	if(line==1)
		lcd_cmd(line1);
	if(line==2)
		lcd_cmd(line2);
		while(pos!=0)					//first position or home position is 0 and last position is 15
		{
		lcd_cmd(0x14);					//move to right 
		pos--;
		}
	while(*senpoint!='\0')
	{
	lcd_char(*senpoint);					// send the character to be displayed
	senpoint++;
	}
}

void LCDWriteInt(int val,unsigned int field_length)
{	
		
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) lcddata('-');
	for(i=j;i<5;i++)
	{
	lcd_char(48+str[i]);
	}
	
}
void lcd_write_int(int val,unsigned int field_length)
{	
		
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) lcddata('-');
	for(i=j;i<5;i++)
	{
	lcd_char(48+str[i]);
	}
	
}

void lcd_scroll_1(unsigned char *s)
{
//	unsigned char i;
	lcd_cmd(0x07);
	lcd_cmd(0x8f);
	lcd_cmd(0x18);
	while(*s)            // While we havent seen a \0 (esc) go on
	{
		lcd_char(*s);            // Send 1st char to our char function
		s++;
		_delay_ms(200);
	}
	
}

#endif
