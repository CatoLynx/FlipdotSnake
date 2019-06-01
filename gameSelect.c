/*
 * gameSelect.c
 *
 * Created: 01.06.2019 19:43:47
 *  Author: Julian
 */ 

#include "gameSelect.h"
#include "game.h"
#include "graphics.h"

t_game selectedGame = SELECT_GAME;

void gameSelectLoop() {
	uint8_t selectionChanged = 1;
	selectedGame = SNAKE;
	while(1) {
		t_direction dir = getDPad();
		
		switch(dir) {
			case UP: {
				if(selectedGame > SNAKE) {
					selectedGame--;
					selectionChanged = 1;
				}
				break;
			}
			case DOWN: {
				if(selectedGame < TETRIS) {
					selectedGame++;
					selectionChanged = 1;
				}
				break;
			}
			case RIGHT: {
				return;
			}
		}
		
		if(selectionChanged) {
			selectionChanged = 0;
			clearPlayfield();
			switch(selectedGame) {
				case SELECT_GAME:
				case SNAKE: {
					drawString(&playfield, 0, 0, ">SNAKE", 1);
					drawString(&playfield, 0, 7, " TETRIS", 1);
					break;
				}
				case TETRIS: {
					drawString(&playfield, 0, 0, " SNAKE", 1);
					drawString(&playfield, 0, 7, ">TETRIS", 1);
					break;
				}
			}
			outputPlayfield();
		}
	}
}