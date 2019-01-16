/*
 * graphics.c
 *
 * Created: 10.01.2019 18:28:41
 *  Author: Julian
 */ 

#include "graphics.h"
#include "flipdot.h"

uint8_t FONT[10*3] = {
	0b11111, 0b10001, 0b11111, // 0
	0b00000, 0b11111, 0b00000, // 1
	0b10111, 0b10101, 0b11101, // 2
	0b10001, 0b10101, 0b11111, // 3
	0b11100, 0b00100, 0b11111, // 4
	0b11101, 0b10101, 0b10111, // 5
	0b11111, 0b10101, 0b10111, // 6
	0b10000, 0b10000, 0b11111, // 7
	0b11111, 0b10101, 0b11111, // 8
	0b11101, 0b10101, 0b11111  // 9
};

void setPixelAt(unsigned int* buf, uint8_t x, uint8_t y, uint8_t state) {
	// Sets a pixel in a framebuffer to a value
	if(x >= MATRIX_WIDTH || y >= VIEWPORT_HEIGHT) return;
	y = MATRIX_HEIGHT - y - 1;
	if(state) {
		buf[x] |= (1 << y);
		} else {
		buf[x] &= ~(1 << y);
	}
}

uint8_t getPixelAt(unsigned int* buf, uint8_t x, uint8_t y) {
	// Gets a pixel in a framebuffer
	if(x >= MATRIX_WIDTH || y >= VIEWPORT_HEIGHT) return 0;
	y = MATRIX_HEIGHT - y - 1;
	return !!(buf[x] & (1 << y));
}

void drawLine(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state) {
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	
	while (1) {
		setPixelAt(buf, x0, y0, state);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

void drawRectangle(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state) {
	drawLine(buf, x0, y0, x0, y1, state);
	drawLine(buf, x0, y1, x1, y1, state);
	drawLine(buf, x1, y1, x1, y0, state);
	drawLine(buf, x1, y0, x0, y0, state);
}

void drawCircle(unsigned int* buf, int x0, int y0, int r, uint8_t state) {
	int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */
	do {
		setPixelAt(buf, x0-x, y0+y, state); /*   I. Quadrant */
		setPixelAt(buf, x0-y, y0-x, state); /*  II. Quadrant */
		setPixelAt(buf, x0+x, y0-y, state); /* III. Quadrant */
		setPixelAt(buf, x0+y, y0+x, state); /*  IV. Quadrant */
		r = err;
		if (r >  x) err += ++x*2+1; /* e_xy+e_x > 0 */
		if (r <= y) err += ++y*2+1; /* e_xy+e_y < 0 */
	} while (x < 0);
}

void drawEllipse(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state) {
	int a = abs (x1 - x0), b = abs (y1 - y0), b1 = b & 1; /* values of diameter */
	long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
	long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

	if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	y0 += (b + 1) / 2;
	y1 = y0-b1;   /* starting pixel */
	a *= 8 * a; b1 = 8 * b * b;
	do
	{
		setPixelAt(buf, x1, y0, state); /*   I. Quadrant */
		setPixelAt(buf, x0, y0, state); /*  II. Quadrant */
		setPixelAt(buf, x0, y1, state); /* III. Quadrant */
		setPixelAt(buf, x1, y1, state); /*  IV. Quadrant */
		e2 = 2 * err;
		if (e2 >= dx)
		{
			x0++;
			x1--;
			err += dx += b1;
			} /* x step */
			if (e2 <= dy)
			{
				y0++;
				y1--;
				err += dy += a;
				}  /* y step */
			} while (x0 <= x1);
			while (y0-y1 < b)
			{  /* too early stop of flat ellipses a=1 */
				setPixelAt(buf, x0-1, y0, state); /* -> finish tip of ellipse */
				setPixelAt(buf, x1+1, y0++, state);
				setPixelAt(buf, x0-1, y1, state);
				setPixelAt(buf, x1+1, y1--, state);
			}
}

void floodFill(unsigned int* buf, int x0, int y0, uint8_t state) {
	if(getPixelAt(buf, x0, y0) != state)
	{
		setPixelAt(buf, x0, y0, state);
		floodFill(buf, x0+1, y0, state);
		floodFill(buf, x0, y0+1, state);
		floodFill(buf, x0-1, y0, state);
		floodFill(buf, x0, y0-1, state);
	}
}

void drawNumber(unsigned int* buf, int x0, int y0, unsigned int number, uint8_t state) {
	uint8_t forceDraw = 0;
	if(forceDraw || number >= 10000) {
		drawNumber(buf, x0, y0, number/10000, state);
		number %= 10000;
		x0 += 4;
		forceDraw = 1;
	}
	if(forceDraw || number >= 1000) {
		drawNumber(buf, x0, y0, number/1000, state);
		number %= 1000;
		x0 += 4;
		forceDraw = 1;
	}
	if(forceDraw || number >= 100) {
		drawNumber(buf, x0, y0, number/100, state);
		number %= 100;
		x0 += 4;
		forceDraw = 1;
	}
	if(forceDraw || number >= 10) {
		drawNumber(buf, x0, y0, number/10, state);
		number %= 10;
		x0 += 4;
		forceDraw = 1;
	}
	uint8_t fontIndex = number * 3;
	for(uint8_t col = 0; col < 3; col++) {
		for(uint8_t row = 0; row < 5; row++) {
			setPixelAt(buf, x0 + col, y0 + row, state ^ !(FONT[fontIndex + col] & (1 << (4 - row))));
		}
	}
}

uint8_t isPointOnLine(uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	// Checks whether the point (x,y) is on the line (x0,y0 - x1,y1)
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	
	while (1) {
		if(x == x0 && y == y0) return 1;
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
	return 0;
}