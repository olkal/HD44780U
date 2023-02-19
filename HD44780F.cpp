
#include <Arduino.h>
#include "HD44780F.h"
#include "HD44780F_HW.h"

// Constructor
HD44780F::HD44780F() {
}

//** initialization procedure
void HD44780F::begin(uint8_t cols, uint8_t lines) {

	RS_PINM_OUT;
	EN_PINM_OUT;
	D4D7_PINM_OUT;
	RS_L;
	EN_L;

	if (RW_PIN_CONNECTED == 1) {
		RW_PINM_OUT;
		RW_L;
	}

	uint8_t functionset = 0;

	functionset = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

	_numlines = lines;
	_cols = cols;

	//**Instructions for initilizing, data sheet page 45-46
	//**Wait for more than 40 ms after VCC rises to 2.7 V
	delay(100);
	//**first time
	write4bits_4bitMode(0x03);
	delayMicroseconds(4500);  // wait >4.1ms
	//**second time
	write4bits_4bitMode(0x03);
	delayMicroseconds(150);
	//**third time
	write4bits_4bitMode(0x03);
	delayMicroseconds(150);
	//**set to 4-bit interface:
	write4bits_4bitMode(0x02);
	delayMicroseconds(150);

	//**set # lines, font size, etc.
	sendCommand(LCD_FUNCTIONSET | functionset);
	//delayMicroseconds(60);
	delayMicroseconds(100);

	//noDisplay();
	//**turn the display
	display();
	delayMicroseconds(100);

	//**clear the LCD
	clear();

	//**Initialize to default text direction (for roman languages)
	displaycontrol = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	//**set the entry mode
	sendCommand(LCD_ENTRYMODESET | displaycontrol);
	delayMicroseconds(100);
}

//** turns off the display
void HD44780F::noDisplay() {
	displaycontrol &= ~LCD_DISPLAYON;
	sendCommand(LCD_DISPLAYCONTROL | displaycontrol);
}

//** turns on the display, cursor off, blink off
void HD44780F::display() {
	displaycontrol |= LCD_DISPLAYON;
	sendCommand(LCD_DISPLAYCONTROL | displaycontrol);
}

//** clear screen, takes 2000us
void HD44780F::clear()
{
	sendCommand(LCD_CLEARDISPLAY);             // clear display, set cursor position to zero
	delayMicroseconds(WAIT_HOME_CLEAR);    // this command is time consuming
}

//** create special character
void HD44780F::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7;            // we only have 8 locations 0-7

	sendCommand(LCD_SETCGRAMADDR | (location << 3));
	delayMicroseconds(30);

	for (uint8_t i = 0; i < 8; i++)
	{
		write(charmap[i]);      // call the virtual write method
		delayMicroseconds(40);
	}
}

//** create special character, for AVR, progmem
#ifdef __AVR__
void HD44780F::createChar(uint8_t location, const char* charmap) {
	location &= 0x7;   // we only have 8 memory locations 0-7

	sendCommand(LCD_SETCGRAMADDR | (location << 3));
	delayMicroseconds(30);

	for (uint8_t i = 0; i < 8; i++)
	{
		write(pgm_read_byte_near(charmap++));
		delayMicroseconds(40);
	}
}
#endif // __AVR__

//** position the cursor, col and row start at 0
void HD44780F::setCursor(uint8_t col, uint8_t row) {
	const uint8_t row_offsetsDef[] = { 0x00, 0x40, 0x14, 0x54 }; // For regular LCDs
	//const uint8_t row_offsetsLarge[] = { 0x00, 0x40, 0x10, 0x50 }; // For 16x4 LCDs, not implemented
	sendCommand(LCD_SETDDRAMADDR | (col + row_offsetsDef[row]));
}

//** for "write" command
size_t HD44780F::write(uint8_t value)
{
	sendData(value);
	return 1;             // assume OK
}

//** send command to LCD
void HD44780F::sendCommand(uint8_t value) {
	busy();
	RS_L;
	WAIT60NS;
	write8bits_4bitMode(value);
	busyStart();
}

//** send data to LCD
void HD44780F::sendData(uint8_t value) {
	busy();
	RS_H;
	WAIT60NS;
	write8bits_4bitMode(value);
	busyStart();
}

//** start busy period (if RW pin not connected)
void HD44780F::busyStart() {
#ifndef USE_READ_BUSY
	waitStart = micros();
#endif
}

//** check busy status
void HD44780F::busy() {
	//**RW pin not connected, using fixed period for busy status
#ifndef USE_READ_BUSY
	while ((micros() - waitStart) < WAIT_BUSY) {
		NOP;
	}
#else
	//** RW pin connected, read DB7 for busy status
	D4D7_PINM_INP;  // set MCU pins to input for DB7-DB04
	RS_L; //RS pin low
	EN_L; //EN pin low
	RW_H; //RW pin high
	WAIT60NS;
	uint8_t flag = 0x80;
	while ((flag & 0x80) == 0x80) { //wait for busy flag to clear
		flag = read8bits_4bitMode();
	}
	RW_L; //RW pin low
	D4D7_PINM_OUT;  // set MCU pins to output for DB7-DB04
#endif
}

//** pulse EN pin
void HD44780F::pulse_EN() {
	EN_H; //EN pin high
	WAIT450NS;
	EN_L; //EN pin low
	WAIT450NS;
}

//** write 4 bits, for initialization procedure in 4bit mode
void HD44780F::write4bits_4bitMode(uint8_t value) {
	if (value & 0x01) D4_H;
	else D4_L;
	if (value & 0x02) D5_H;
	else D5_L;
	if (value & 0x04) D6_H;
	else D6_L;
	if (value & 0x08) D7_H;
	else D7_L;
	pulse_EN();
}

//** write 8 bits, 4bit mode
void HD44780F::write8bits_4bitMode(uint8_t value) {
	if (value & 0x80) D7_H;
	else D7_L;
	if (value & 0x40) D6_H;
	else D6_L;
	if (value & 0x20) D5_H;
	else D5_L;
	if (value & 0x10) D4_H;
	else D4_L;
	pulse_EN();
	if (value & 0x08) D7_H;
	else D7_L;
	if (value & 0x04) D6_H;
	else D6_L;
	if (value & 0x02) D5_H;
	else D5_L;
	if (value & 0x01) D4_H;
	else D4_L;
	pulse_EN();
}

//** Read 8 bits, 4bit mode (RW pin must be connected)
//** It's not required to read more than D7 for busy status, but 
//** as reading the port directly is so fast, it dosent seem to make any difference in speed
uint8_t HD44780F::read8bits_4bitMode() {
	uint8_t val = 0;
	EN_H; //EN pin high
	WAIT450NS;
	if (D7_READ) val |= 0x80;
	if (D6_READ) val |= 0x40;
	if (D5_READ) val |= 0x20;
	if (D4_READ) val |= 0x10;
	EN_L; //EN pin low
	WAIT450NS;
	EN_H; //EN pin high
	WAIT450NS;
	if (D7_READ) val |= 0x08;
	if (D6_READ) val |= 0x04;
	if (D5_READ) val |= 0x02;
	if (D4_READ) val |= 0x01;
	EN_L; //EN pin low
	return val;
}
