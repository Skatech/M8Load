/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.07.27
 * Text menu library source file
 *****************************************************************************/

#include <avr\io.h>
//#include <util\delay.h>
#include <avr\interrupt.h>
#include "config.h"
#include "LCD8814.h"
#include "menu.h"

// Currently pressed or released key code or zero otherwise
uint8_t g_mnu_lastkey = 0;
// Currently pressed key holding time in KEY_POLL_DELAY fractions
uint8_t g_mnu_keyhold = 0;
// Buzzer activity time in 500us fractions
volatile uint16_t g_mnu_buzzing = 0;
// Milliseconds resolution counter, lasting 49.17:02:47.296
volatile uint32_t g_mnu_mstimer = 0;

void UI_Clock() {
}

ISR(TIMER0_OVF_vect) { // to adjust period modify TCNT0
	static uint16_t msfrac; // intermediate fractions to mitigate timer 0xff overflow
	msfrac += 0xff;
	if (msfrac >= 500) {
		msfrac -= 500;
		g_mnu_mstimer++;
	}

	if (g_mnu_buzzing) {		
		if (g_mnu_buzzing--) {
			BUZZER_PORT ^= _BV(BUZZER_PIN);
		}
		//else BUZZER_PORT &= ~_BV(BUZZER_PIN);
	}
}

void UI_Sleep(uint16_t ms) {
	uint32_t start = g_mnu_mstimer;
	while(ms > (uint16_t)(g_mnu_mstimer - start)) {
		asm("nop");
	}
}

void UI_Idle() {
 	static uint32_t last;
	
	// uint16_t rest = KEY_POLL_DELAY - (uint16_t)(g_mnu_mstimer - last); // 7376
	// if (rest < KEY_POLL_DELAY) {
	// 	UI_Sleep(rest);
	// }
	
 	while (KEY_POLL_DELAY > (uint16_t)(g_mnu_mstimer - last)) {
 		asm("nop");
 	}

 	last = g_mnu_mstimer;
}

void UI_Buzz(uint16_t ms) {
	uint16_t clocks = ms * 2;
	if (g_mnu_buzzing < clocks) {
		g_mnu_buzzing = clocks;
	}
}

void UI_Initialize() {
	#ifndef KEYBOARD_PORT
		#error KEYBOARD_PORT must be defined in config.h
	#endif
	#ifndef KEYBOARD_DDRS
		#error KEYBOARD_DDRS must be defined in config.h
	#endif
	#ifndef KEYBOARD_PINS
		#error KEYBOARD_PINS must be defined in config.h
	#endif
	#ifndef KEYBOARD_PIN_UP
		#error KEYBOARD_PIN_UP must be defined in config.h
	#endif
	#ifndef KEYBOARD_PIN_OK
		#error KEYBOARD_PIN_OK must be defined in config.h
	#endif
	#ifndef KEYBOARD_PIN_DN
		#error KEYBOARD_PIN_DN must be defined in config.h
	#endif
	#ifndef BUZZER_PORT
		#error BUZZER_PORT must be defined in config.h
	#endif
	#ifndef BUZZER_DDRS
		#error BUZZER_DDRS must be defined in config.h
	#endif
	#ifndef BUZZER_PIN
		#error BUZZER_PIN must be defined in config.h
	#endif

	// keyboard inputs
	KEYBOARD_DDRS &= ~(_BV(KEYBOARD_PIN_UP) | _BV(KEYBOARD_PIN_DN) | _BV(KEYBOARD_PIN_OK)); // input directions
	KEYBOARD_PORT = _BV(KEYBOARD_PIN_UP) | _BV(KEYBOARD_PIN_DN) | _BV(KEYBOARD_PIN_OK); // internal pullup

	// buzzer output
	BUZZER_DDRS |= _BV(BUZZER_PIN); 
	BUZZER_PORT &= ~_BV(BUZZER_PIN);

	// system timer configuration
	TCCR0 = _BV(CS01);  // T0 prescaler CS00-CS02: 0:stop 1:1 2:8 3:64 4:256 5:1024 6:T0pin-fall 7:T0pin-rise
	TIMSK = _BV(TOIE0); // T0 overflow interrupt
}

//	Preceding key exists
//		Same with current -> increasing hold counter
//			else -> mark preceding as released using zero hold counter
//		else if key pressed -> registering new keypress
//      	else -> cleaning

// Returns KEY_PRESSED_XX and KEY_RELEASED_XX codes for just pressed and released keys,
// KEY_XX codes for holding, otherwise zero. Updates g_mnu_lastkey and g_mnu_keyhold global variables
uint8_t ScanKeys() {
	uint8_t key	= 0;
	switch((_BV(KEYBOARD_PIN_UP) | _BV(KEYBOARD_PIN_OK) | _BV(KEYBOARD_PIN_DN)) & (~KEYBOARD_PINS)) {
		case _BV(KEYBOARD_PIN_UP):
		 	key = KEY_UP;
			break;
		case _BV(KEYBOARD_PIN_OK):
		 	key = KEY_OK;
			break;
		case _BV(KEYBOARD_PIN_DN):
		 	key = KEY_DN;
	}
	
	if (g_mnu_keyhold && g_mnu_lastkey) {
		if (key == g_mnu_lastkey) {
			if (g_mnu_keyhold < 0xff) { // increasing hold counter
				g_mnu_keyhold++; 
			}
		}
		else g_mnu_keyhold = 0; // mark preceding as released using zero hold counter
	}
	else if (key) {
		g_mnu_lastkey = key; // registering new keypress
		g_mnu_keyhold = 1;
		UI_Buzz(25);
	}
	else g_mnu_lastkey = g_mnu_keyhold = 0;  // cleaning

	return g_mnu_lastkey | (g_mnu_lastkey
		? g_mnu_keyhold < 1 ? KEY_RELEASED
            : g_mnu_keyhold < 2 ? KEY_PRESSED : 0 : 0);
}

// Returns value accelerated increment or decrement amount for different key holding timings
uint8_t GetKeyAcceleration(uint8_t speed2, uint8_t speed3) {
	if (g_mnu_keyhold == 0x01) {
		return 1;
	}
	if (g_mnu_keyhold == 0xff) {
		return speed3;
	}
	if (g_mnu_keyhold == 0x00) {
		return 0;
	}
	if (g_mnu_keyhold & 0x03) {
		return 0;
	}
	if (g_mnu_keyhold >= 0x24) {
		return speed3;
	}
	if (g_mnu_keyhold >= 0x14) {
		return speed2;
	}
	return 1;
	//return g_key_holding > 0x20 ? speed3 : g_key_holding > 0x10 ? speed2 : g_key_holding > 0 ? 1 : 0;
}

// Returns value accelerated increment bounded result for different key holding timings
uint16_t AcceleratedIncrement(uint16_t value, uint16_t maximum, uint8_t speed2, uint8_t speed3) {
	uint8_t inc = GetKeyAcceleration(speed2, speed3);
	return maximum - value > inc ? value + inc : maximum;
}

// Returns value accelerated decrement bounded result for different key holding timings
uint16_t AcceleratedDecrement(uint16_t value, uint16_t minimum, uint8_t speed2, uint8_t speed3) {
	uint8_t dec = GetKeyAcceleration(speed2, speed3);
	return value - minimum > dec ? value - dec : minimum;
}

uint8_t ShowMenu(const char** items, uint8_t count, uint8_t top, uint8_t cursor) {
	for (uint8_t i = 0; i < count; i++) { // draw menu items
		LCD_set_position(6, top + i);
		LCD_draw_string(items[i]);
	}

	while(ScanKeys()) { // wait keys released
		UI_Idle();
	}

	for (uint8_t i = 0;; ++i) {	// menu iterations and item select loop
		switch(ScanKeys()) {
			case KEY_PRESSED_UP:
				cursor = cursor-- ? cursor : count - 1;
				break;
			case KEY_PRESSED_DN:
				cursor = ++cursor < count ? cursor : 0;
				break;	
			case KEY_RELEASED_OK:
				return cursor;
		}

		LCD_set_position(0, top + cursor);
		LCD_draw_char(i & 0x08 ? ' ' : '>');
		UI_Idle();
		LCD_set_position(0, top + cursor);
		LCD_draw_char(' ');
	}
}

uint8_t FormatNumber(char* buff, uint8_t value, uint8_t options) {
	uint8_t i = 0;
	if (options > 2) {
		buff[i++] = '0' + value / 100;
	}
	if (options > 1) {
		buff[i++] = '0' + (value % 100) / 10;
	}
	buff[i++] = '0' + (value % 10);	
	
	return i;
}

uint8_t FormatNumberW(char* buff, uint16_t value, uint8_t width, char filler) {
	uint8_t pos = 0;
	for (int8_t pow = 4; pow >= 0; pow -= 1) {
		uint16_t div = pow > 3 ? 10000 : pow > 2 ? 1000 : pow > 1 ? 100 : pow > 0 ? 10 : 1;
		uint8_t dig = value / div;
		value %= div;
		if (dig > 0 || pow < 1) {
			buff[pos++] = '0' + dig;
			filler = '0';
			width = pow;
		}
		else if (pow < width) {
			buff[pos++] = filler;
		}
	}
	return pos;
}

uint16_t SetNumberOptionW(char* buffer, uint8_t offset, uint8_t width, uint8_t ypos,
 		uint16_t value, uint16_t minimum, uint16_t maximum, uint8_t speed2, uint8_t speed3) {
	//uint8_t speed2 = (maximum < 0x100) ? 2 : 5;
	//uint8_t speed3 = (maximum < 0x100) ? 5 : 25;

	for (uint8_t i = 0;; ++i) {
		switch(ScanKeys()) {
			case KEY_UP:
			case KEY_PRESSED_UP:
				value = AcceleratedIncrement(value, maximum, speed2, speed3);
				break;
			case KEY_DN:
			case KEY_PRESSED_DN:
				value = AcceleratedDecrement(value, minimum, speed2, speed3);
				break;			
			case KEY_RELEASED_OK:
				return value;
		}

		LCD_set_position(0, ypos);
		LCD_draw_char(i & 0x08 ? ' ' : '*');
		FormatNumberW(buffer + offset, value, width, '.');
		LCD_draw_string(buffer);
		UI_Idle();
	}
}

void FormatBooleanOption(char* buffer, uint8_t option) {
	const char* str = option ? "Yes" : "No ";
	for(int i = 0; str[i]; i++) {
		buffer[i] = str[i];
	}
}

