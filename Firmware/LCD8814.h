/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.07.27
 * LCD Nokia1110i driver header file
 *****************************************************************************/

#include <avr/io.h>
#include "config.h"

// Also possible commands: swap ram msb/lsb order (A8, A9),
//  vertical or horizontal mode (AA, AB), horizontal reverse (A1, A9)
#define LCD8814_VREV_OFF 0xC0 // Display vertical reverse off command
#define LCD8814_VREV_ON  0xC8 // Display vertical reverse on command
#define LCD8814_INVR_OFF 0xA6 // Display inversion off command
#define LCD8814_INVR_ON  0xA7 // Display inversion on command

void LCD_send_command(uint8_t data);
void LCD_send_data(uint8_t data);
void LCD_draw_char(char code);
void LCD_draw_string(const char* text);
void LCD_set_position(uint8_t xpos, uint8_t ypos);
void LCD_clear_screen();
void LCD_FillRectange(uint8_t data, uint8_t xpos, uint8_t ypos, uint8_t width, uint8_t height);
void LCD_initialize();
void LCD_mode_positive(uint8_t mode);

void LCD_draw_string_X2(const char* text, uint8_t xpos, uint8_t ypos);
void LCD_draw_char_4X(char code, uint8_t xpos, uint8_t ypos);
void LCD_draw_string_4X(const char* text, uint8_t xpos, uint8_t ypos);
