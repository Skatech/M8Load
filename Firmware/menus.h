/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.08.05
 * Program menu functions
 *****************************************************************************/

#ifndef __MENUS_H__
#define __MENUS_H__

#include <avr\io.h>

// Set display modes from bit-mask flags
void SetDisplayMode(uint8_t mode);

// Main menu function
void ShowMenu_Options();

#endif