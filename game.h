/*
 * game.h
 *
 * Created: 30.05.2019 15:36:58
 *  Author: Julian
 */ 


#ifndef GAME_H_
#define GAME_H_

#include "flipdot.h"

uint16_t playfield[MATRIX_WIDTH];
uint16_t oldPlayfield[MATRIX_WIDTH];

typedef enum e_directions {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	INVALID
} t_direction;

t_direction getDPad();
uint8_t getRandomNumber(uint8_t min, uint8_t max);
void clearPlayfield();
void overlayPlayfield(uint16_t* buf);
void outputPlayfield();
void restoreOldPlayfield();

#endif /* GAME_H_ */