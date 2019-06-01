/*
 * flipdot.h
 *
 * Created: 09.01.2019 20:26:03
 *  Author: Julian
 */ 


#ifndef FLIPDOT_H_
#define FLIPDOT_H_

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

/*
 * MATRIX CONFIGURATION
 */

#define MATRIX_WIDTH 28
#define MATRIX_HEIGHT 16
#define MIN_ROW 1
#define MAX_ROW 13
#define VIEWPORT_HEIGHT (MAX_ROW - MIN_ROW + 1)
#define PANEL_WIDTH 28

/*
 * PIN DECLARATIONS
 */

#define PORT_ROW_A0 PORTD
#define PORT_ROW_A1 PORTD
#define PORT_ROW_A2 PORTD
#define PORT_ROW_A3 PORTD
#define PORT_COL_A0 PORTC
#define PORT_COL_A1 PORTC
#define PORT_COL_A2 PORTC
#define PORT_COL_A3 PORTC
#define PORT_COL_A4 PORTB
#define PORT_ROWS_BOTTOM PORTD
#define PORT_ROWS_TOP PORTD
#define PORT_D PORTC
#define PORT_E PORTB
#define PORT_LED PORTC

#define ROW_A0 (1 << PD6)
#define ROW_A1 (1 << PD7)
#define ROW_A2 (1 << PD5)
#define ROW_A3 (1 << PD4)
#define COL_A0 (1 << PC4)
#define COL_A1 (1 << PC3)
#define COL_A2 (1 << PC2)
#define COL_A3 (1 << PC0)
#define COL_A4 (1 << PB5)
#define ROWS_BOTTOM (1 << PD3)
#define ROWS_TOP (1 << PD2)
#define D (1 << PC1)
#define E1 (1 << PB3)
#define E2 (1 << PB0)
#define E3 (1 << PB1)
#define E4 (1 << PB2)
#define E5 (1 << PB4)
#define LED (1 << PC5)

/*
 * GLOBAL CONSTANTS
 */

#define BAUDRATE 57600
#define SERIAL_TIMEOUT 5000
#define FLIP_DURATION 1000 // in microseconds
#define FLIP_PAUSE_DURATION 250 // in microseconds

enum SERIAL_STATUSES {
  SUCCESS = 0xFF,
  TIMEOUT = 0xE0,
  ERROR = 0xEE,
};

void setPin(uint8_t* port, uint8_t pin, uint8_t state);
void selectColumn(uint8_t colIndex);
void selectRow(uint8_t rowIndex, uint8_t yellow);
void deselect();
void flip(uint8_t panelIndex);
void setBacklight(uint8_t status);
void setPixel(uint8_t x, uint8_t y, uint8_t state);
void setMatrix(unsigned int* newBitmap, unsigned int* oldBitmap);
void clearMatrix();

#endif /* FLIPDOT_H_ */