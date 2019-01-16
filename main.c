/*
 * FlipdotSnake.c
 *
 * Created: 09.01.2019 18:50:24
 * Author : Julian
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "flipdot.h"
#include "snake.h"

extern unsigned int* playfield;
extern t_direction curDirection, lastDirection;
extern int curXPos, curYPos;
extern unsigned int curSnakeLength;


int main(void)
{
	setPin(&PORT_ROWS_BOTTOM, ROWS_BOTTOM, 1);
	setPin(&PORT_ROWS_TOP, ROWS_TOP, 1);
	
	DDRB |= 0b00111111;
	DDRC |= 0b00111111;
	DDRD |= 0b11111100;
	
	setPin(&PORT_LED, LED, 1);
	
	ADMUX = 0b01000110;
	ADCSRA |= 0b11100111;
	unsigned int randomSeed = 0;
	uint8_t dummy = 0;
	for(uint8_t i = 0; i < 10; i++) {
		randomSeed |= ((unsigned int)ADCL & 1) << i;
		dummy = ADCH;
		_delay_ms(1);
	}
	srand(randomSeed);
	ADMUX = 0b01000111;
	
	t_collisionType collisionType = NONE;
	
	clearMatrix();
	
	while(1) {
		clearPlayfield();
		resetSnake();
		clearPlayfield();
		renderSnake();
		outputPlayfield();
		while(getDPad() == INVALID);
	
		while (1) 
		{
			clearPlayfield();
			generateObjects();
			collisionType = advanceSnake();
			if(collisionType != NONE) {
				endGame();
				while(getDPad() == INVALID);
				break;
			}
			renderSnake();
			outputPlayfield();
		}
	
	}
}
