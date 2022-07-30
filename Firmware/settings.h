#include <avr/eeprom.h>

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// Version of required settings data format
#define EEPROM_SETTINGS_VERSION 0x10
#define EEPROM_ERROR_INCOMPATIBLE_VERSION 0xFF

// Loading settings from EEPROM, returns error code or zero on success
uint8_t EEPROM_LoadSettings();
// Saving settings to EEPROM
void EEPROM_SaveSettings();

#endif