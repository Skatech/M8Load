#include <avr/eeprom.h>

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

extern uint16_t s_off_vmin;
extern uint8_t s_lcd_mode, s_adc_mvpc, s_adc_mapc;

// Version of required settings data format
#define EEPROM_SETTINGS_VERSION 0x12
#define EEPROM_ERROR_INCOMPATIBLE_VERSION 0xFF

// Loading settings from EEPROM, returns error code or zero on success
uint8_t EEPROM_LoadSettings();
// Saving settings to EEPROM
void EEPROM_SaveSettings();

#endif