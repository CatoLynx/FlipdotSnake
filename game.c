/*
 * game.c
 *
 * Created: 30.05.2019 15:36:48
 *  Author: Julian
 */ 

#include "game.h"
#include <avr/io.h>
#include <stdlib.h>

uint16_t playfield[MATRIX_WIDTH] = {0};
uint16_t oldPlayfield[MATRIX_WIDTH] = {0};
	
t_direction getDPad() {
	uint16_t adcResult = ADCL;
	adcResult |= ((uint16_t)ADCH << 8);
	if(adcResult < 20) return UP;
	if(adcResult > 300 && adcResult < 350) return DOWN;
	if(adcResult > 490 && adcResult < 530) return LEFT;
	if(adcResult > 830 && adcResult < 870) return RIGHT;
	return INVALID;
}

uint8_t getRandomNumber(uint8_t min, uint8_t max) {
	return ((uint32_t)rand() * (max - min) / RAND_MAX) + min;
}

void clearPlayfield() {
	memset(&playfield, 0, MATRIX_WIDTH*2);
}

void overlayPlayfield(uint16_t* buf) {
	for(uint16_t i = 0; i < MATRIX_WIDTH; i++) {
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