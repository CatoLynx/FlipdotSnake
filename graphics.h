/*
 * graphics.h
 *
 * Created: 10.01.2019 18:28:33
 *  Author: Julian
 */ 


#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>

void setPixelAt(unsigned int* buf, uint8_t x, uint8_t y, uint8_t state);
uint8_t getPixelAt(unsigned int* buf, uint8_t x, uint8_t y);
void drawLine(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
void drawRectangle(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
void drawCircle(unsigned int* buf, int x0, int y0, int r, uint8_t state);
void drawEllipse(unsigned int* buf, int x0, int y0, int x1, int y1, uint8_t state);
void floodFill(unsigned int* buf, int x0, int y0, uint8_t state);
void drawNumber(unsigned int* buf, int x0, int y0, unsigned int number, uint8_t state);
uint8_t isPointOnLine(uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

#endif /* GRAPHICS_H_ */