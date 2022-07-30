/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.07.27
 * Text menu library header file
 *****************************************************************************/
#include <avr\io.h>

#define KEY_UP          0x01                    // Keycode of UP key
#define KEY_OK          0x02                    // Keycode of SELECT key
#define KEY_DN          0x03                    // Keycode of DOWN key

#define KEY_PRESSED     0x04                    // Keycode just pressed decoration flag
#define KEY_RELEASED    0x08                    // Keycode just released decoration flag

#define KEY_PRESSED_UP  KEY_UP | KEY_PRESSED    // Just pressed UP key
#define KEY_PRESSED_OK  KEY_OK | KEY_PRESSED    // Just pressed SELECT key
#define KEY_PRESSED_DN  KEY_DN | KEY_PRESSED    // Just pressed DOWN key
#define KEY_RELEASED_UP KEY_UP | KEY_RELEASED   // Just released UP key
#define KEY_RELEASED_OK KEY_OK | KEY_RELEASED   // Just released SELECT key
#define KEY_RELEASED_DN KEY_DN | KEY_RELEASED   // Just released DOWN key

#define KEY_POLL_DELAY  50	// Period in milliseconds between key polls and screen updates

void UI_Initialize();
void UI_Sleep(uint16_t ms);
void UI_Buzz(uint16_t ms);
void UI_Idle();

// Returns KEY_PRESSED_XX and KEY_RELEASED_XX codes for just pressed and released keys,
// KEY_XX codes for holding, otherwise zero. Updates g_key_current and g_key_holding global variables
uint8_t ScanKeys();

// Returns value accelerated increment or decrement amount for different key holding timings
uint8_t GetKeyAcceleration(uint8_t speed2, uint8_t speed3);
// Returns value accelerated increment bounded result for different key holding timings
uint16_t AcceleratedIncrement(uint16_t value, uint16_t maximum, uint8_t speed2, uint8_t speed3);
// Returns value accelerated decrement bounded result for different key holding timings
uint16_t AcceleratedDecrement(uint16_t value, uint16_t minimum, uint8_t speed2, uint8_t speed3);

uint8_t ShowMenu(const char** items, uint8_t count, uint8_t top, uint8_t cursor);;

uint8_t FormatNumber(char* buff, uint8_t value, uint8_t options);
uint8_t SetNumberOption(char* buff, uint8_t disp, uint8_t ypos, uint8_t value, uint8_t minimum, uint8_t maximum);

uint8_t FormatNumberW(char* buff, uint16_t value, uint8_t width, char filler);
uint16_t SetNumberOptionW(char* buffer, uint8_t offset, uint8_t width, uint8_t ypos,
 		uint16_t value, uint16_t minimum, uint16_t maximum, uint8_t speed2, uint8_t speed3);

void FormatBooleanOption(char* buff, uint8_t value);
uint8_t SetBitFlagOption(char* buff, uint8_t offset, uint8_t ypos, uint8_t value, uint8_t mask);



