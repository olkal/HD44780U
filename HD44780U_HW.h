#ifndef HD44780FHW_H
#define HD44780FHW_H

//** Hardware configuration file.

#define NOP __asm__ __volatile__("nop\n\t")

//** Delays according to data sheet. delayMicroseconds() don't have enough resolution, better to use NOP.
//** Must be configured to your mcu frequency. Values below are ok for 16mhz AVR.
#ifdef __AVR__
//** 450nS delay:
#define WAIT450NS __asm__ __volatile__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
//** 60nS delay:
#define WAIT60NS NOP
#endif

//** RW pin and busy flag config
#define RW_PIN_CONNECTED 1 // 0 == RW pin not connected, 1 == RW pin connected
#define USE_READ_BUSY //un-comment to use fixed BUSY delay (37us)

//** Hardware macros. Edit the macros below to suit your mcu type and your wiring.
//** For very fast MCU's I guess the generic Arduino functions like pinMode() digitalRead() and digitalWrite() will be okay,
//** but for e.g. 16mhz AVR's direct port manipulation is much faster.
#define RS_PINM_OUT DDRC |= B10000000 //RS PC07 pinmode
#define RW_PINM_OUT DDRD |= B10000000 //RW PD07 pinmode
#define EN_PINM_OUT DDRC |= B01000000 //EN PC06 pinmode
#define D4D7_PINM_OUT DDRC |= B00111100 //D4-D7 PC02-PC05 pinmode output (write operation)
#define D4D7_PINM_INP DDRC &= B11000011 //D4-D7 PC02-PC05 pinmode input (to read D7 busy flag), all 4 pins must be set to input mode
#define D7_READ ((PINC & B00000100) >> 2) //D7 PC02 read
#define D7_L PORTC &= B11111011 //D7 PC02 low
#define D7_H PORTC |= B00000100 //D7 PC02 high
#define D6_L PORTC &= B11110111 //D6 PC03 low
#define D6_H PORTC |= B00001000 //D6 PC03 high
#define D5_L PORTC &= B11101111 //D5 PC04 low
#define D5_H PORTC |= B00010000 //D5 PC04 high
#define D4_L PORTC &= B11011111 //D4 PC05 low
#define D4_H PORTC |= B00100000 //D4 PC05 high
#define EN_L PORTC &= B10111111 //EN PC06 low
#define EN_H PORTC |= B01000000 //EN PC06 high
#define RS_L PORTC &= B01111111 //RS PC07 low
#define RS_H PORTC |= B10000000 //RS PC07 high 
#define RW_L PORTD &= B01111111 //RW PD07 low
#define RW_H PORTD |= B10000000 //RW PD07 high

#endif
