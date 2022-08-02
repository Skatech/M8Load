#include <avr/eeprom.h>
#include "config.h"
#include "settings.h"

uint8_t  EEMEM EEPROM_STORED_VERSION = EEPROM_SETTINGS_VERSION;
uint8_t  EEMEM EEPROM_CPU_FCAL = 0x7F;
uint8_t  EEMEM EEPROM_LCD_MODE = DISPLAY_DEFAULTS;
uint8_t  EEMEM EEPROM_ADC_MVPC = 30;
uint8_t  EEMEM EEPROM_ADC_MAPC = 3;
uint16_t EEMEM EEPROM_OFF_VMIN = 3000;

// Display mode flags
uint8_t s_lcd_mode = 0;
// Voltage adc millivolts per count
uint8_t s_adc_mvpc = 30;
// Current adc milliamperes per count
uint8_t s_adc_mapc = 3;
// Minimum input voltage in millivolts, before load switches off
uint16_t s_off_vmin = 3000;

void EEPROM_SaveSettings() {
	eeprom_write_byte(&EEPROM_STORED_VERSION, EEPROM_SETTINGS_VERSION);
	eeprom_write_byte(&EEPROM_CPU_FCAL, OSCCAL);
	eeprom_write_byte(&EEPROM_LCD_MODE, s_lcd_mode);
	eeprom_write_byte(&EEPROM_ADC_MVPC, s_adc_mvpc);
	eeprom_write_byte(&EEPROM_ADC_MAPC, s_adc_mapc);
	eeprom_write_word(&EEPROM_OFF_VMIN, s_off_vmin);
}

uint8_t EEPROM_LoadSettings() {
	if (EEPROM_SETTINGS_VERSION == eeprom_read_byte(&EEPROM_STORED_VERSION)) {
		OSCCAL	   = eeprom_read_byte(&EEPROM_CPU_FCAL);
		s_lcd_mode = eeprom_read_byte(&EEPROM_LCD_MODE);
		s_adc_mvpc = eeprom_read_byte(&EEPROM_ADC_MVPC);
		s_adc_mapc = eeprom_read_byte(&EEPROM_ADC_MAPC);
		s_off_vmin = eeprom_read_word(&EEPROM_OFF_VMIN);
		return 0;
	}
	return EEPROM_ERROR_INCOMPATIBLE_VERSION;
}
