/*
 * flipdot.c
 *
 * Created: 09.01.2019 20:24:56
 *  Author: Julian
 */

#include "flipdot.h"
#include <stdint.h>

// MATRIX CONFIGURATION
uint8_t PANEL_LINES[1] = {0};

uint8_t hasHalfPanelOffset(uint8_t colIndex) {
  /*
   * Determine if the given column index belongs to a panel which was preceded by a 14-col half panel.
   */

  return 0;
}

// The associations between address and actually selected matrix row are not 1:1, so we need to use a lookup table
// The values in this array represent the address needed to drive the row corresponding to the array index
uint8_t ROW_TABLE_BLACK[8] = {10, 11, 8, 9, 14, 15, 12, 13}; // Black = lowside
uint8_t ROW_TABLE_YELLOW[8] = {7, 6, 5, 4, 3, 1, 2, 0}; // Yellow = highside
uint8_t E_LINES[5] = {E2, E2, E3, E4, E5}; // E1 is usually first but it's placed on MOSI and prevents ISP from working, so for a single panel I jumpered it to use E2

uint8_t matrixClean = 0;
uint8_t pixelInverting = 0;
uint8_t activeState = 1;
uint8_t quickUpdate = 1;

/*
 * HELPER FUNCTIONS
 */

void setQuickUpdate(uint8_t state) {
	quickUpdate = state;
}

void setPin(uint8_t* port, uint8_t pin, uint8_t state) {
	if(state) {
		*port |= pin;
	} else {
		*port &= ~pin;
	}
}

/*
 * PROGRAM CODE
 */

void selectColumn(uint8_t colIndex) {
  /*
   * Select the appropriate panel for the specified column index and set the column address pins accordingly.
   */

  // In the case of a matrix with a 14-col panel at the end instead of a 28-col one, we need to remember that our panel index is off by half a panel, so flip the MSB
  uint8_t halfPanelOffset = hasHalfPanelOffset(colIndex);

  // Additionally, the address needs to be reversed because of how the panels are connected
  colIndex = MATRIX_WIDTH - colIndex - 1;

  // Since addresses start from the beginning in every panel, we need to wrap around after reaching the end of a panel
  uint8_t address = colIndex % PANEL_WIDTH;

  // A quirk of the FP2800 chip used to drive the columns is that addresses divisible by 8 are not used, so we need to skip those
  address += (address / 7) + 1;

  setPin(&PORT_COL_A0, COL_A0, address & 1);
  setPin(&PORT_COL_A1, COL_A1, address & 2);
  setPin(&PORT_COL_A2, COL_A2, address & 4);
  setPin(&PORT_COL_A3, COL_A3, address & 8);
  setPin(&PORT_COL_A4, COL_A4, halfPanelOffset ? !(address & 16) : address & 16);
}

void selectRow(uint8_t rowIndex, uint8_t yellow) {
  /*
   * Select the specified row by writing its address to the line decoders and selecting the appropriate line decoder.
   * This function uses the row lookup tables to determine the address for the desired row number.
   * The yellow parameter tells the function which lookup table to use.
   */
  
  rowIndex += MIN_ROW; // Compensate for shortened matrices

  // Determine and output the address
  uint8_t address = yellow ? ROW_TABLE_YELLOW[rowIndex % 8] : ROW_TABLE_BLACK[rowIndex % 8];
  setPin(&PORT_ROW_A0, ROW_A0, address & 1);
  setPin(&PORT_ROW_A1, ROW_A1, address & 2);
  setPin(&PORT_ROW_A2, ROW_A2, address & 4);
  setPin(&PORT_ROW_A3, ROW_A3, address & 8);

  // Determine which line decoder to use
  if (rowIndex <= 7) {
    // Use the line decoder for the top 8 rows
    setPin(&PORT_ROWS_BOTTOM, ROWS_BOTTOM, 1);
    setPin(&PORT_ROWS_TOP, ROWS_TOP, 0);
  } else {
    // Use the line decoder for the bottom 8 rows
    setPin(&PORT_ROWS_TOP, ROWS_TOP, 1);
    setPin(&PORT_ROWS_BOTTOM, ROWS_BOTTOM, 0);
  }

  // Set the D line of the FP2800 chip according to the yellow parameter (highside for black, lowside for yellow)
  setPin(&PORT_D, D, !yellow);
}

void deselect() {
  /*
   * Deselect all rows by inhibiting the line decoders
   * and deselect the column by setting col address 0
   */

  setPin(&PORT_ROWS_BOTTOM, ROWS_BOTTOM, 1);
  setPin(&PORT_ROWS_TOP, ROWS_TOP, 1);

  setPin(&PORT_ROW_A0, ROW_A0, 0);
  setPin(&PORT_ROW_A1, ROW_A1, 0);
  setPin(&PORT_ROW_A2, ROW_A2, 0);
  setPin(&PORT_ROW_A3, ROW_A3, 0);

  setPin(&PORT_COL_A0, COL_A0, 0);
  setPin(&PORT_COL_A1, COL_A1, 0);
  setPin(&PORT_COL_A2, COL_A2, 0);
  setPin(&PORT_COL_A3, COL_A3, 0);
  setPin(&PORT_COL_A4, COL_A4, 0);
}

void flip(uint8_t panelIndex) {
  /*
   * Send an impulse to the specified panel to flip the currently selected dot.
   */

  // Get the enable line for the specified panel
  uint8_t e = E_LINES[PANEL_LINES[panelIndex]];

  setPin(&PORT_E, e, 1);
  _delay_us(FLIP_DURATION);
  setPin(&PORT_E, e, 0);
  _delay_us(FLIP_PAUSE_DURATION);
}

void setBacklight(uint8_t status) {
  /*
   * Enable or disable the LED backlight of the matrix.
   */

  setPin(&PORT_LED, LED, !status);
}

void setPixel(uint8_t x, uint8_t y, uint8_t state) {
	selectColumn(y);
	selectRow(x, state);
	flip(x / PANEL_WIDTH);
	deselect();
}

void setMatrix(uint16_t* newBitmap, uint16_t* oldBitmap) {
  /*
   * Write a bitmap to the matrix.
   */

  for (int16_t col = 0; col < MATRIX_WIDTH; col++) {
    uint16_t newColData = newBitmap[col];
    uint16_t oldColData;
    uint8_t colChanged;

    // Determine whether the current column has been changed
    if (quickUpdate && oldBitmap) {
      // We're in delta mode, compare the two bitmaps and refresh only the pixels that have changed
      oldColData = oldBitmap[col];
      colChanged = newColData != oldColData;
    } else {
      // We don't have anything to compare or Quick Update is disabled, so just do a full refresh
      oldColData = 0x0000;
      colChanged = 1;
    }
    if (!colChanged) continue;

    // Which panel are we on?
    uint8_t panel = col / PANEL_WIDTH;

    selectColumn(col);
    for (int16_t row = 0; row < MATRIX_HEIGHT; row++) {
      // Determine whether the current pixel has been changed
      uint8_t pixelChanged = !quickUpdate || !oldBitmap || (oldColData & (1 << row)) != (newColData & (1 << row));
      if (!pixelChanged) continue;
      uint8_t newPixelValue = !!(newColData & (1 << row));
      newPixelValue ^= pixelInverting;
      selectRow(MATRIX_HEIGHT - row - 1, newPixelValue);
      flip(panel);
    }
  }
  deselect();
  if (!oldBitmap) matrixClean = 1;
}

void clearMatrix() {
	for (int16_t col = 0; col < MATRIX_WIDTH; col++) {
		// Which panel are we on?
		uint8_t panel = col / PANEL_WIDTH;

		selectColumn(col);
		for (int16_t row = 0; row < MATRIX_HEIGHT; row++) {
			selectRow(MATRIX_HEIGHT - row - 1, 0);
			flip(panel);
		}
	}
	deselect();
}