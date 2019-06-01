/*
 * graphics.h
 *
 * Created: 10.01.2019 18:28:33
 *  Author: Julian
 */ 


#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>

typedef enum e_collisions {
	NO_COLL = 0,
	PIXEL = 1,
	OOB_TOP = 2,
	OOB_BOTTOM = 4,
	OOB_LEFT = 8,
	OOB_RIGHT = 16
} t_collision;

const uint8_t FONT[10*3];

t_collision setPixelAt(unsigned int* buf, int x, int y, uint8_t state);
uint8_t getPixelAt(unsigned int* buf, uint8_t x, uint8_t y);
t_collision drawLine(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
t_collision drawRectangle(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
t_collision drawCircle(unsigned int* buf, int x0, int y0, int r, uint8_t state);
t_collision drawEllipse(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
void floodFill(unsigned int* buf, int x0, int y0, uint8_t state);
t_collision drawBitmapColMajor(unsigned int* buf, int x0, int y0, uint8_t width, uint8_t height, uint8_t* bitmap, uint8_t state, uint8_t useAlpha);
t_collision drawBitmapRowMajor(unsigned int* buf, int x0, int y0, uint8_t width, uint8_t height, uint8_t* bitmap, uint8_t state, uint8_t useAlpha);
t_collision drawNumber(unsigned int* buf, int x0, int y0, unsigned int number, uint8_t state);
uint8_t isPointOnLine(uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif /* GRAPHICS_H_ */