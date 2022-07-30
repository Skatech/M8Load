/******************************************************************************
 * (c) Skatech Research Lab, 2000-2022.
 * Last change: 2022.07.26
 * Project configuration file
 *****************************************************************************/

// comment this line when not use encoder
//#define ENCODER

// connection details of keyboard
#define KEYBOARD_PORT	PORTD
#define KEYBOARD_DDRS	DDRD
#define KEYBOARD_PINS	PIND
#define KEYBOARD_PIN_UP	PD5
#define KEYBOARD_PIN_OK	PD6
#define KEYBOARD_PIN_DN	PD7

// connection details of buzzer
#define BUZZER_PORT	    PORTB
#define BUZZER_DDRS     DDRB
#define BUZZER_PIN      PB0

// connection details of LCD
#define LCD_PORT PORTB
#define LCD_DDRS DDRB
#define LCD_PINS PINB
#define LCD_PINCLC PB3
#define LCD_PINSDA PB4
#define LCD_PINCSX PB5

// ADC configuration
#define ADMUX_REFSOURCE _BV(REFS0) | _BV(REFS1)
#define ADMUX_CHANNELV _BV(MUX1) | _BV(MUX2)
#define ADMUX_CHANNELC _BV(MUX0) | _BV(MUX1) | _BV(MUX2)
