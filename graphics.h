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

t_collision setPixelAt(uint16_t* buf, int16_t x, int16_t y, uint8_t state);
uint8_t getPixelAt(uint16_t* buf, uint8_t x, uint8_t y);
t_collision drawLine(uint16_t* buf, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t state);
t_collision drawRectangle(uint16_t* buf, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t state);
void fill(uint16_t* buf, uint8_t state, uint16_t length);
t_collision drawCircle(uint16_t* buf, int16_t x0, int16_t y0, int16_t r, uint8_t state);
t_collision drawEllipse(uint16_t* buf, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t state);
void floodFill(uint16_t* buf, int16_t x0, int16_t y0, uint8_t state);
t_collision drawBitmapColMajor(uint16_t* buf, int16_t x0, int16_t y0, uint8_t width, uint8_t height, uint8_t* bitmap, uint8_t state, uint8_t useAlpha);
t_collision drawBitmapRowMajor(uint16_t* buf, int16_t x0, int16_t y0, uint8_t width, uint8_t height, uint8_t* bitmap, uint8_t state, uint8_t useAlpha);
t_collision drawString(uint16_t* buf, int16_t x0, int16_t y0, char* string, uint8_t state);
t_collision drawNumber(uint16_t* buf, int16_t x0, int16_t y0, uint32_t number, uint8_t state);
uint8_t isPointOnLine(uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif /* GRAPHICS_H_ */