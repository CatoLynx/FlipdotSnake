/*
 * game.c
 *
 * Created: 30.05.2019 15:36:48
 *  Author: Julian
 */ 

#include "game.h"
#include <avr/io.h>
#include <stdlib.h>

unsigned int playfield[MATRIX_WIDTH] = {0};
unsigned int oldPlayfield[MATRIX_WIDTH] = {0};
	
t_direction getDPad() {
	unsigned int adcResult = ADCL;
	adcResult |= ((unsigned int)ADCH << 8);
	if(adcResult < 20) return UP;
	if(adcResult > 300 && adcResult < 350) return DOWN;
	if(adcResult > 490 && adcResult < 530) return LEFT;
	if(adcResult > 830 && adcResult < 870) return RIGHT;
	return INVALID;
}

uint8_t getRandomNumber(uint8_t min, uint8_t max) {
	return ((unsigned long)rand() * (max - min) / RAND_MAX) + min;
}

void clearPlayfield() {
	memset(&playfield, 0, MATRIX_WIDTH*2);
}

void overlayPlayfield(unsigned int* buf) {
	for(unsigned int i = 0; i < MATRIX_WIDTH; i++) {
		playfield[i] |= buf[i];
	}
}

void outputPlayfield() {
	setMatrix(&playfield, &oldPlayfield);
	memcpy(&oldPlayfield, &playfield, MATRIX_WIDTH*2);
}

void restoreOldPlayfield() {
	memcpy(&playfield, &oldPlayfield, MATRIX_WIDTH*2);
}