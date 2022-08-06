/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.08.05
 * Program menu functions
 *****************************************************************************/

#include "config.h"
#include "settings.h"
#include "LCD8814.h"
#include "menu.h"

#ifndef DISPLAY_VREV
	#error DISPLAY_VREV must be defined in configuration file
#endif
#ifndef DISPLAY_INVR
	#error DISPLAY_INVR must be defined in configuration file
#endif

void SetDisplayMode(uint8_t mode) {
	LCD_send_command((mode & DISPLAY_VREV) ? LCD8814_VREV_ON : LCD8814_VREV_OFF);
	LCD_send_command((mode & DISPLAY_INVR) ? LCD8814_INVR_ON : LCD8814_INVR_OFF);
}

void ShowMenu_SystemOptions() {
	static char item_voff[]	= "Voff,mv..XXXXX";
	static char item_fcal[]	= "Fcalibr....XXX";
	static const char* menu_items[] = { item_voff, item_fcal, "Return" };
	uint16_t voff = s_off_vmin;
	uint8_t fcal = OSCCAL;
	uint8_t menucur = 0;

	while (1) {
		FormatNumber(item_voff + 9, voff, 5, '.');
		FormatNumber(item_fcal + 11, fcal, 3, '.');
		LCD_clear_screen();
		LCD_draw_string(" System Options");

		switch (menucur = ShowMenu(menu_items, 3, 2, menucur)) {
			case 0:
				voff = SetNumberOption(item_voff, 9, 5, 2, voff, 0, 65000, 50, 500);
				break;
			case 1:
				fcal = SetNumberOption(item_fcal, 11, 3, 3, fcal, 0x00, 0xFF, 5, 25);
				break;
			default:
				s_off_vmin = voff;
				OSCCAL = fcal;
			 	return;
		}
	}
}

void ShowMenu_Display() {
	static char item_rawd[]		= "Rawdata....XXX";
	static char item_vrev[] 	= "V-Reverse..XXX";
	static char item_invr[]		= "Inversion..XXX";
	static const char* menu_items[] = { item_rawd, item_vrev, item_invr, "Return" };
	uint8_t cursor = 0, lcdmode = s_lcd_mode;

	while (1) {
		FormatBooleanOption(item_vrev + 11, lcdmode & DISPLAY_VREV);
		FormatBooleanOption(item_invr + 11, lcdmode & DISPLAY_INVR);
		FormatBooleanOption(item_rawd + 11, lcdmode & DISPLAY_RAWD);
		LCD_clear_screen();
		LCD_draw_string("Display Options");

		switch (cursor = ShowMenu(menu_items, 4, 2, cursor)) {
			case 0:
				lcdmode ^= DISPLAY_RAWD;
				break;
			case 1:
				lcdmode ^= DISPLAY_VREV;
				break;
			case 2:
				lcdmode ^= DISPLAY_INVR;
				break;
			default:
				//SetDisplayMode(s_lcd_mode = lcdmode);
				s_lcd_mode = lcdmode;
			 	return;
		}
		SetDisplayMode(lcdmode);
	}
}

void ShowMenu_ADCOptions() {
	static char item_mvpc[]		= "V mv/cnt..XX";
	static char item_mapc[] 	= "A ma/cnt..XX";

	static const char* menu_items[] = { item_mvpc, item_mapc, "Return" };
	uint8_t mvpc = s_adc_mvpc;
	uint8_t mapc = s_adc_mapc;
	uint8_t menucur = 0;

	while (1) {
		FormatNumber(item_mvpc + 10, mvpc, 2, '.');
		FormatNumber(item_mapc + 10, mapc, 2, '.');

		LCD_clear_screen();
		LCD_draw_string(" Measure Options");

		switch (menucur = ShowMenu(menu_items, 3, 2, menucur)) {
			case 0:
				mvpc = SetNumberOption(item_mvpc, 10, 2, 2, mvpc, 0, 64, 2, 5);
				break;
			case 1:
				mapc = SetNumberOption(item_mapc, 10, 2, 3, mapc, 0, 64, 2, 5);
				break;
			default:
				s_adc_mvpc = mvpc;
				s_adc_mapc = mapc;
			 	return;
		}
	}
}

void ShowMenu_Options() {
	static const char* menu_title = " Options";
	static const char* menu_items[] = { "System..", "Display..", "Measure..", "Save", "Return" };
	uint8_t menucur = 0;

	while (1) {
		LCD_clear_screen();
		LCD_draw_string(menu_title);

		switch (menucur = ShowMenu(menu_items, 5, 2, menucur)) {
			case 0:
				ShowMenu_SystemOptions();
				break;
			case 1:
				ShowMenu_Display();
				break;
			case 2:
				ShowMenu_ADCOptions();
				break;
			case 3:
				EEPROM_SaveSettings();
			default: return;
		}
	}
}
