/*
 * FlipdotSnake.c
 *
 * Created: 09.01.2019 18:50:24
 * Author : Julian
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "game.h"
#include "flipdot.h"
#include "snake.h"
#include "tetris.h"
#include "gameSelect.h"

volatile uint64_t sysTicks = 0;

ISR(TIMER0_COMPA_vect) {
	// Systick interrupt
	sysTicks++;
}

int16_t main(void)
{
	setPin(&PORT_ROWS_BOTTOM, ROWS_BOTTOM, 1);
	setPin(&PORT_ROWS_TOP, ROWS_TOP, 1);
	
	DDRB |= 0b00111111;
	DDRC |= 0b00111111;
	DDRD |= 0b11111100;
	
	setPin(&PORT_LED, LED, 1);
	
	ADMUX = 0b01000110;
	ADCSRA |= 0b11100111;
	uint16_t randomSeed = 0;
	uint8_t dummy = 0;
	for(uint8_t i = 0; i < 10; i++) {
		randomSeed |= ((uint16_t)ADCL & 1) << i;
		dummy = ADCH;
		_delay_ms(1);
	}
	srand(randomSeed);
	ADMUX = 0b01000111;
	
	TCCR0A = 0b00000010;	// CTC mode
	TCCR0B = 0b00000011;	// Clock source: F_CPU/64 (= 250 kHz)
	OCR0A  = 249;			// 1 ms
	TIMSK0 = 0b00000010;	// Enable interrupt
	
	asm("sei");
	
	clearMatrix();
	
	switch(selectedGame) {
		case SNAKE: {
			snakeInit();
			break;
		}
		case TETRIS: {
			tetrisInit();
			break;
		}
	}
	
	while(1) {
		switch(selectedGame) {
			case SELECT_GAME: {
				gameSelectLoop();
				break;
			}
			case SNAKE: {
				snakeLoop();
				selectedGame = SELECT_GAME;
				break;
			}
			case TETRIS: {
				tetrisLoop();
				selectedGame = SELECT_GAME;
				break;
			}
		}
		while(getDPad() == INVALID);
		while(getDPad() != INVALID);
	}
}
