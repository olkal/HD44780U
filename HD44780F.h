#ifndef HD44780F_H
#define HD44780F_H

#include "Arduino.h"
#include "avr/pgmspace.h"
//#include <inttypes.h>

// LCD Commands
// ---------------------------------------------------------------------------
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

// flags for display entry mode
// ---------------------------------------------------------------------------
#define LCD_ENTRYRIGHT          0x00 // I/D
#define LCD_ENTRYLEFT           0x02 // I/D
#define LCD_ENTRYSHIFTINCREMENT 0x01 // S
#define LCD_ENTRYSHIFTDECREMENT 0x00 // S

// flags for display on/off and cursor control
// ---------------------------------------------------------------------------
#define LCD_DISPLAYON           0x04 // D
#define LCD_DISPLAYOFF          0x00 // D
#define LCD_CURSORON            0x02 // C
#define LCD_CURSOROFF           0x00 // C
#define LCD_BLINKON             0x01 // B
#define LCD_BLINKOFF            0x00 // B

// flags for display/cursor shift
// ---------------------------------------------------------------------------
#define LCD_DISPLAYMOVE         0x08 // S/C
#define LCD_CURSORMOVE          0x00 // S/C
#define LCD_MOVERIGHT           0x04 // R/L
#define LCD_MOVELEFT            0x00 // R/L

// flags for function set
// ---------------------------------------------------------------------------
#define LCD_8BITMODE            0x10 //DL == 1: 8 bits
#define LCD_4BITMODE            0x00 //DL == 0: 4 bits
#define LCD_2LINE               0x08 //N == 1: 2 line
#define LCD_1LINE               0x00 //N == 0: 1 line
#define LCD_5x10DOTS            0x04 //F
#define LCD_5x8DOTS             0x00 //F

// Define COMMAND and DATA LCD Rs (used by send method).
// ---------------------------------------------------------------------------
#define COMMAND                 0
#define LCD_DATA                1
#define FOUR_BITS               2

#define NOP __asm__ __volatile__("nop\n\t")

#ifdef __AVR__
#define WAIT450NS __asm__ __volatile__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define WAIT60NS NOP
#endif

#define WAIT_HOME_CLEAR			2000
#define WAIT_BUSY				37

//** hardware macros
#define RW_PIN_CONNECTED 1
#define USE_READ_BUSY
#define RS_PINM_OUT DDRC |= B10000000
#define RW_PINM_OUT DDRD |= B10000000
#define EN_PINM_OUT DDRC |= B01000000
#define D4D7_PINM_OUT DDRC |= B00111100 //PC02-PC05 output
#define D4D7_PINM_INP DDRC &= B11000011 //PC02-PC05 input
#define D4D7_L PORTC &= B11000011
#define D7_READ ((PINC & B00000100) >> 2) //PC02 read
#define D6_READ ((PINC & B00001000) >> 3) //PC03 read
#define D5_READ ((PINC & B00010000) >> 4) //PC04 read
#define D4_READ ((PINC & B00100000) >> 5) //PC05 read
#define D7_L PORTC &= B11111011 //PC02 low
#define D7_H PORTC |= B00000100 //PC02 high
#define D6_L PORTC &= B11110111 //PC03 low
#define D6_H PORTC |= B00001000 //PC03 high
#define D5_L PORTC &= B11101111 //PC04 low
#define D5_H PORTC |= B00010000 //PC04 high
#define D4_L PORTC &= B11011111 //PC05 low
#define D4_H PORTC |= B00100000 //PC05 high
#define EN_L PORTC &= B10111111 //PC06 low
#define EN_H PORTC |= B01000000 //PC06 high
#define RS_L PORTC &= B01111111 //PC07 low
#define RS_H PORTC |= B10000000 //PC07 high 
#define RW_L PORTD &= B01111111 //PD07 low
#define RW_H PORTD |= B10000000 //PD07 high

class HD44780F : public Print
{
public:
	HD44780F();
	//** LCD initialization
	void begin(uint8_t cols, uint8_t lines);

	//** clears the LCD screen and positions the cursor in the upper-left corner (time consuming).
	void clear();

	//** turns off the display
	void noDisplay();

	//** turns on the display
	void display();
	/*
	//** display the blinking LCD cursor
	void blink();

	//** hides the LCD cursor
	void noBlink();

	//** Scrolls the contents of the display (text and cursor) one space to the left
	void scrollDisplayLeft();

	//** Scrolls the contents of the display (text and cursor) one space to the right
	void scrollDisplayRight();

	//** set the direction for text written to the LCD to left-to-right
	void leftToRight();

	//** set the direction for text written to the LCD to right-to-left
	void rightToLeft();

	//** moves the cursor one space to the left
	void moveCursorLeft();

	//** moves the cursor one space to the right
	void moveCursorRight();

	//** enable autoscroll will outputting each new character to the same location on the LCD.
	void autoscroll();

	//** disable autoscroll
	void noAutoscroll();
	*/

	//** create special character
	void createChar(uint8_t location, uint8_t charmap[]);

	//** create special character, for AVR, progmem
#ifdef __AVR__
	void createChar(uint8_t location, const char* charmap);
#endif // __AVR__

	//** position the cursor
	void setCursor(uint8_t col, uint8_t row);

	//** virtual write method, implemented in the Print class
	virtual size_t write(uint8_t value);
	using Print::write;

private:
	uint8_t displaycontrol = 0;   // LCD base control command LCD on/off, blink, cursor
	uint8_t displaymode = 0;      // Text entry mode to the LCD
	uint8_t _numlines = 0;         // Number of lines of the LCD, initialized with begin()
	uint8_t _cols = 0;             // Number of columns in the LCD
	uint32_t waitStart = 0;			// start of busy period


	void sendCommand(uint8_t value);

	void sendData(uint8_t value);

	void busyStart();

	void busy();

	void pulse_EN();

	void write4bits_4bitMode(uint8_t value);

	void write8bits_4bitMode(uint8_t val);

	uint8_t read8bits_4bitMode();
};


#endif

