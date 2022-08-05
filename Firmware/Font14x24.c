/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.08.05
 * LCD Nokia1110i driver source file
 *****************************************************************************/

#include <avr/pgmspace.h>
#include "LCD8814.h"

static uint8_t __fontH3[] PROGMEM = {
	//0 12x24,
	0xF8,0xF8,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0xE7,0xE7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0xE7,
	0x1F,0x1F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//1 12x24,
	0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0xE7,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,
	//2 12x24,
	0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0xE0,0xE0,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x07,0x07,
	0x1F,0x1F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,
	//3 12x24,
	0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE7,0xE7,
	0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//4 12x24,
	0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,
	0x07,0x07,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE7,0xE7,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,
	//5 12x24,
	0xF8,0xF8,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00,0x00,
	0x07,0x07,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE0,0xE0,
	0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//6 12x24,
	0xF8,0xF8,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00,0x00,
	0xE7,0xE7,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE0,0xE0,
	0x1F,0x1F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//7 12x24,
	0x00,0x00,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0xE7,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,
	//8 12x24,
	0xF8,0xF8,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0xE7,0xE7,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE7,0xE7,
	0x1F,0x1F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//9 12x24,
	0xF8,0xF8,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF8,0xF8,
	0x07,0x07,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xE7,0xE7,
	0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x1F,0x1F,
	//A 8x24,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*0x00,0x00,0x00,0x00,*/
	0xF0,0xF8,0x0C,0x06,0x06,0x0C,0xF8,0xF0,/*0x00,0x00,0x00,0x00,*/
	0x7F,0x7F,0x03,0x03,0x03,0x03,0x7F,0x7F,/*0x00,0x00,0x00,0x00,*/
	//V 8x24,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*0x00,0x00,0x00,0x00,*/
	0xFE,0xFE,0x00,0x00,0x00,0x00,0xFE,0xFE,/*0x00,0x00,0x00,0x00,*/
	0x0F,0x1F,0x30,0x60,0x60,0x30,0x1F,0x0F,/*0x00,0x00,0x00,0x00,*/
	//Dot 2x24,
	0x00,0x00,/*0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,*/
	0x00,0x00,/*0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,*/
	0x60,0x60/*,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,*/
};

uint8_t LCD_DrawChar14X24(char code, uint8_t xpos, uint8_t ypos)
{
	uint16_t offs;
	uint8_t wdth;

	switch (code) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			wdth = 12;
			offs = 12 * 3 * (code - '0');
			break;
		case 'A':
			wdth = 8;
			offs = 12 * 3 * 10 + 8 * 3 * 0;
			break;
		case 'V':
			wdth = 8;
			offs = 12 * 3 * 10 + 8 * 3 * 1;
			break;
		case '.':
			wdth = 2;
			offs = 12 * 3 * 10 + 8 * 3 * 2;
			break;
		default:
			return xpos;
	}

  	for (uint8_t b = 0; b < 3; b++)
  	{
		LCD_set_position(xpos, ypos + b);
	  	for (uint8_t i = 0; i < wdth; i++)
	  	{
			LCD_send_data(pgm_read_byte(&__fontH3[offs + b * wdth + i]));
		}
		LCD_send_data(0);
		LCD_send_data(0);
	}
	return xpos + wdth + 2;
}

void LCD_DrawString14X24(const char* text, uint8_t xpos, uint8_t ypos)
{
	for (uint8_t i = 0; text[i]; i++)
	{
		xpos = LCD_DrawChar14X24(text[i], xpos, ypos);
	}
}
