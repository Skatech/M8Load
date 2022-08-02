//#define SIMULATION

#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr/sleep.h>
//#include <util\delay.h>
//#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include "config.h"
#include "settings.h"
#include "LCD8814.h"
#include "menu.h"

// main screen menu modes
#define MSMMODE_NORM	0x00
#define MSMMODE_ADJV	0x01
#define MSMMODE_ADJC	0x02
#define MSM_ADJDELAY	80

// Persistent settigns varibles
extern uint16_t s_off_vmin;
extern uint8_t s_lcd_mode, s_adc_mvpc, s_adc_mapc;

// Menu system variables
extern uint8_t g_mnu_keyhold;
extern uint32_t g_mnu_mstimer;

void SetDisplayMode(uint8_t mode) {
	//LCD_send_command(hrev ? 0xA9 : 0xA8); // swap ram msb/lsb order (A8, A9)
	//LCD_send_command(hrev ? 0xAB : 0xAA); // vertical or horizontal mode (AA, AB)
	//LCD_send_command((mode & DISPLAY_HREV) ? 0xA9 : 0xA1); // horizontal reverse (A1, A9)
	LCD_send_command((mode & DISPLAY_VREV) ? 0xC8 : 0xC0); // display vertical reverse (C7, C8)
	LCD_send_command((mode & DISPLAY_INVR) ? 0xA7 : 0xA6); // positive mode (A6, A7)
}

void LCD_DrawString14X24(const char* text, uint8_t xpos, uint8_t ypos);
void LCD_DrawString14X32(const char* text, uint8_t xpos, uint8_t ypos);

void FormatADC3(char* buff, uint16_t adc, char code) {
	uint8_t i = 0, d = 100, p = adc < 9995 ? 10 : 1;

	adc += p == 10 ? 5 : 50;
	adc /= p == 10 ? 10 : 100;

	while (1) {
		if (d == p) {
			buff[i++] = '.';
		}
		if (d == 1) {
			buff[i++] = '0' + (uint8_t)adc;
			buff[i++] = code;
			buff[i] = '\0';
			break;
		}
		buff[i++] = '0' + (uint8_t)(adc / d);
		adc %= d;
		d /= 10;
	}
}

void FormatTime(char* buffer, uint32_t ms) {
	uint8_t hh = 0, mm = 0, ss = 0;
	// hh = ms / 3600000U;
	// ms %= 3600000U;
	// mm = ms / 60000U;
	// ms %= 60000U;
	// ss = ms / 1000U;
	while(ms >= 3600000U) {
		ms -= 3600000U;
		hh++;
	}
	while(ms >= 60000U) {
		ms -= 60000U;
		mm++;
	}
	while(ms >= 1000U) {
		ms -= 1000U;
		ss++;
	}
	FormatNumber(buffer + 0, hh, 2, '0');
	buffer[2] = ':';
	FormatNumber(buffer + 3, mm, 2, '0');
	buffer[5] = ':';
	FormatNumber(buffer + 6, ss, 2, '0');
	buffer[8] = '\0';
}

void FormatHex(char buffer[], uint8_t number) {
	for (uint8_t i = 0; i < 2; i++) {
		uint8_t x = (i) ? (number & 0x0F) : (number >> 0x04);
		buffer[i] = (x < 0x0A) ? '0' + x : 'A' - 0x0A + x;
	}
}

uint8_t PutText(char* buff, const char* value, uint8_t offset) {
	for(uint8_t n = 0; value[n]; n++) {
		buff[offset++] = value[n];
	}
	return offset;
}

void ShowMenu_ADCOptions() {
	//static char menu_subt[]		= "(XXXX/XX.XX)";
	static char item_mvpc[]		= "V mv/cnt..XX";
	static char item_mapc[] 	= "A ma/cnt..XX";

	static const char* menu_items[] = { item_mvpc, item_mapc, "Return" };
	uint8_t mvpc = s_adc_mvpc;
	uint8_t mapc = s_adc_mapc;
	uint8_t menucur = 0;

	while (1) {
		//FormatADC3(menu_subt + 6, *pnval, nmin, nmax, dmax, ')');
		//FormatNumberW(menu_subt + 1, *pnval, 4, '0');
		FormatNumber(item_mvpc + 10, mvpc, 2, '.');
		FormatNumber(item_mapc + 10, mapc, 2, '.');

		LCD_clear_screen();
		LCD_draw_string(" Measure Options");
		//LCD_set_position(6, 1);
		//LCD_draw_string(menu_subt);

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
	uint8_t disp = s_lcd_mode;
	uint8_t menucur = 0;

	while (1) {
		FormatBooleanOption(item_vrev + 11, disp & DISPLAY_VREV);
		FormatBooleanOption(item_invr + 11, disp & DISPLAY_INVR);
		FormatBooleanOption(item_rawd + 11, disp & DISPLAY_RAWD);
		LCD_clear_screen();
		LCD_draw_string("Display Options");

		switch (menucur = ShowMenu(menu_items, 4, 2, menucur)) {
			case 0:
				disp ^= DISPLAY_RAWD;
				break;
			case 1:
				disp ^= DISPLAY_VREV;
				break;
			case 2:
				disp ^= DISPLAY_INVR;
				break;
			default:
				SetDisplayMode(s_lcd_mode = disp);
			 return;
		}
		SetDisplayMode(disp);
	}
}

// main menu function
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

// Number of readings to accumulate per ADC-channel before deliver, 0.416ms each reading
#define READINGS_TO_ACCUMULATE 600 // 600 * 2ch * 0.416ms = 0.5s

uint32_t g_adcresv = 0, g_adcresc = 0;  // ADC resulting summs from READINGS_TO_ACCUMULATE readings
uint32_t g_adcaccv = 0, g_adcaccc = 0; // ADC readings accumulators
uint16_t g_adcaccscount = 0; // ADC accumulate readings counter
uint8_t g_adcdeliveries = 0; // ADC unprocessed deliveries counter

uint16_t AccumulatedReadingsToRaw(uint32_t accumulated) {
	return (uint16_t)((accumulated + READINGS_TO_ACCUMULATE / 2) / READINGS_TO_ACCUMULATE);
}

uint16_t AccumulatedReadingsToReal(uint32_t accumulated, uint8_t scale) {
	return (uint16_t)((accumulated * scale + READINGS_TO_ACCUMULATE / 2) / READINGS_TO_ACCUMULATE);
}

ISR(ADC_vect) {
	switch (ADMUX & (ADMUX_CHANNELV | ADMUX_CHANNELC)) {
		case ADMUX_CHANNELV:
			ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELC;
			g_adcaccc += ADC;
			break;

		case ADMUX_CHANNELC:
			ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELV;
			g_adcaccv += ADC;
			break;
	}

	if (++g_adcaccscount == 2 * READINGS_TO_ACCUMULATE) {
		g_adcresv = g_adcaccv;
		g_adcresc = g_adcaccc;
		g_adcdeliveries++;

		g_adcaccscount = 0x00;
		g_adcaccv = g_adcaccc = 0x00;
	}
}

#define REFRESH_OUTV	0x01
#define REFRESH_OUTC	0x02
#define REFRESH_ADJV	0x04
#define REFRESH_ADJC	0x08
#define REFRESH_CNTX	0x10

// #define REFRESH_FLSU	0x10 // flashing updated ???
// #define REFRESH_FLSV	0x20 // flashing visible
// #define REFRESH_STBU	0x40 // stabilizing mode marker updated
// #define REFRESH_STBV	0x80 // stabilizing mode marker visible

int main() {
	uint16_t outv = 0, outc = 0, adjc = 0/*, adjv = 0, ccount = 0*/;
	uint16_t rawv = outv, rawc = outc;
	uint8_t adjustimer = 0, /*msmmode = 0,*/ turnedon = 0, refreshmask = 0xff;
	uint32_t lastmillis = 0, capacity = 0;
	char buffer[20];

	uint8_t eeloaded = EEPROM_LoadSettings();

	// #ifdef SIMULATION
	// 	s_lcd_mode = DISPLAY_VREV | DISPLAY_HREV | DISPLAY_RAWD;
	// #endif

	// initialize LCD
	LCD_initialize();
	SetDisplayMode(/*s_lcd_mode == 0xFF ? s_lcd_mode = 0 :*/ s_lcd_mode);
	LCD_clear_screen();

	// draw title screen
	if (eeloaded == EEPROM_ERROR_INCOMPATIBLE_VERSION) {
		LCD_draw_string("DEFAULT SETTINGS");
	}
	LCD_set_position(1 * 6, 3);
	LCD_draw_string("SKATECH");
	LCD_set_position(3 * 6, 4);
	LCD_draw_string("Research Lab");
	LCD_set_position(0 * 6, 7);
	LCD_draw_string(FIRMWARE_VERSION);

	UI_Initialize();
	sei();
	UI_Buzz(500);
	UI_Sleep(1500);
	LCD_clear_screen();

	// adc setup Tconv = F_CPU / PRESCALE / 13 (2404 reads per sec)
	ADCSRA = _BV(ADIE) | _BV(ADFR) | _BV(ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // ADC interupt, free mode, ADC enabled, prescaler CK/128
	ADMUX = ADMUX_REFSOURCE | ADMUX_CHANNELV; // reference 2.56V, input A0
	ADCSRA |= (_BV(ADSC)); // start conversion

	//PWM initialization (Fast PWM, 10-bit) ?? OCR1A OCR1B
    DDRB = DDRB | (_BV(DDB1) | _BV(DDB2)); //OC1A, OC1B direction output
    TCCR1A = _BV(WGM10) | _BV(WGM11) | _BV(COM1B1) | _BV(COM1A1); //OC1A, OC1B pins - low level on compare match
	TCCR1B = _BV(CS10) | _BV(WGM12); //start timer-counter with no prescaling


	// main loop
	for (;;) {
		if (g_adcdeliveries) {
			uint16_t v = AccumulatedReadingsToReal(g_adcresv, s_adc_mvpc);
			if (v != outv) {
				outv = v;
				rawv = AccumulatedReadingsToRaw(g_adcresv);
				refreshmask = refreshmask | REFRESH_OUTV;
			}

			uint16_t c = AccumulatedReadingsToReal(g_adcresc, s_adc_mapc);
			if (c != outc) {
				outc = c;
				rawc = AccumulatedReadingsToRaw(g_adcresc);
				refreshmask = refreshmask | REFRESH_OUTC;
			}

			g_adcdeliveries = 0x00;
		}

		if (turnedon) {
			if (g_mnu_mstimer - lastmillis >= 1000) {
				lastmillis += 1000;
				//CALCULATE TIME MORE PRECISLY using really elapsed secs!!!!!!!!!!!!!!!!!!!!!!!
				//Move increment closer to current update code !!!!!!!!!!!!!!!!!!!!!!!!
				capacity += outc; // capacity ma/sec  
				refreshmask |= REFRESH_CNTX;
			}
			if (s_off_vmin > outv) {
				turnedon = 0;
				refreshmask |= REFRESH_CNTX;
				UI_Buzz(2500);
			}
		}

		if (turnedon) {
			if (OCR1B != adjc) {
				OCR1B = adjc;
			}
		}
		else {
			if (OCR1B != 0) {
				OCR1B = 0;
			}
		}

		if (refreshmask & REFRESH_OUTV) {
			LCD_set_position(1, 0);
			LCD_draw_string("VOLTAGE");

			// if ((msmmode == MSMMODE_ADJV) && (!(refresh & REFRESH_FLSV))) {
			// 	LCD_FillRectange(0x00, 0, 1, 14 * 3 + 4, 4);
			// }
			// else {
				FormatADC3(buffer, outv, '\0');
				LCD_DrawString14X32(buffer, 0, 1);
			//}
		}
		if (refreshmask & REFRESH_OUTC) {
			LCD_set_position(54, 0);
			LCD_draw_string("CURRENT");

			// if ((msmmode == MSMMODE_ADJC) && (!(refresh & REFRESH_FLSV))) {
			// 	LCD_FillRectange(0x00, 50, 1, 14 * 3 + 4, 4);
			// }
			//else {

				FormatADC3(buffer, adjustimer ? adjc * s_adc_mapc : outc, '\0');
				LCD_DrawString14X32(buffer, 50, 1);
			//}
		}

		// // stabilizing marker
		// if (refresh & REFRESH_STBU) {
		// 	LCD_set_position(34, 5);
		// 	if (stbmode) { // limiting curent
		// 		if (refresh & REFRESH_STBV) {
		// 			LCD_draw_string("LIMIT");
		// 			UI_Buzz(2000);
		// 		} else {
		// 			LCD_draw_string("     ");
		// 		}
		// 	} else { // stabilizing voltage
		// 		LCD_draw_string("STABL");
		// 	}
		// }

		// adjustment rulers
		/*if (refresh & (REFRESH_ADJV | REFRESH_ADJC)) {
			DrawRulersH(
				((msmmode == MSMMODE_ADJV) && (refresh & REFRESH_FLSV)) ? 0 : adjv,
				((msmmode == MSMMODE_ADJC) && (refresh & REFRESH_FLSV)) ? 0 : adjc);
		}*/
		refreshmask |= REFRESH_CNTX;

		if (refreshmask & REFRESH_CNTX) {
			// draw time elapsed
			FormatTime(buffer, lastmillis);
			LCD_set_position(0 * 6, 7);
			LCD_draw_string(buffer);

			// draw discharged capacity
			buffer[PutText(buffer, "mAh", FormatNumber(buffer + 0, capacity / 3600, 6, ' '))] = '\0';
			//buffer[PutText(buffer, "mAh", FormatNumberW(buffer + 0, xxx, 6, ' '))] = '\0';
			LCD_draw_string(buffer);

			LCD_set_position(0 * 6, 6);
			LCD_draw_string(turnedon ? "ON " : "OFF");
		}

		if (s_lcd_mode & DISPLAY_RAWD) {
			if (refreshmask & REFRESH_OUTV) { // outv value raw
				FormatNumber(buffer + 0, rawv, 4, '0');
				buffer[4] = '\0';
				LCD_set_position(0 * 6, 5);
				LCD_draw_string(buffer);
			}
			if (refreshmask & REFRESH_OUTC) { // outc value raw
				FormatNumber(buffer + 0, adjustimer ? adjc : rawc, 4, '0');
				buffer[4] = '\0';
				LCD_set_position(12 * 6, 5);
				LCD_draw_string(buffer);
			}

			/*if (refresh & REFRESH_ADJV) { // outv adjustment raw
				FormatNumberW(buffer + 0, adjv, 4, '0');
				buffer[4] = '\0';
				LCD_set_position(0 * 6, 6);
				LCD_draw_string(((msmmode == MSMMODE_ADJV) && (refresh & REFRESH_FLSV)) ? "    " : buffer);
			}
			if (refresh & REFRESH_ADJC) { // outc adjustment raw
				FormatNumberW(buffer + 0, adjc, 4, '0');
				buffer[4] = '\0';
				LCD_set_position(12 * 6, 6);
				LCD_draw_string(((msmmode == MSMMODE_ADJC) && (refresh & REFRESH_FLSV)) ? "    " : buffer);
			}*/
		}
		
		refreshmask = 0x00;

		if (adjustimer) {
			if (--adjustimer == 0) {
				refreshmask = REFRESH_OUTC;
				UI_Buzz(25);
			}
		}

		// main screen control switch
		switch (ScanKeys()) {
			case KEY_OK:
				if (g_mnu_keyhold > 20) {
					ShowMenu_Options();
					LCD_clear_screen();
					refreshmask = REFRESH_OUTV | REFRESH_OUTC | REFRESH_ADJV | REFRESH_ADJC | REFRESH_CNTX;
					UI_Buzz(50);
				}
				break;

			case KEY_RELEASED_OK:
				if (turnedon) {
					turnedon = 0;
					UI_Buzz(50);
				}
				else {
					turnedon = 1;
					g_mnu_mstimer = lastmillis = capacity = 0;
					UI_Buzz(250);
				}
				refreshmask = REFRESH_CNTX;
				break;

			case KEY_UP:
			case KEY_PRESSED_UP:
				adjc = AcceleratedIncrement(adjc, 1000, 5, 25);
				adjustimer = MSM_ADJDELAY;
				refreshmask |= REFRESH_OUTC;
				break;

			case KEY_DN:
			case KEY_PRESSED_DN:
				adjc = AcceleratedDecrement(adjc, 0, 5, 25);
				adjustimer = MSM_ADJDELAY;
				refreshmask |= REFRESH_OUTC;
				break;
		}
		
		UI_Idle();
	}
}
