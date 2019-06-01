/*
 * gameSelect.h
 *
 * Created: 01.06.2019 19:44:00
 *  Author: Julian
 */ 


#ifndef GAMESELECT_H_
#define GAMESELECT_H_

#include <stdint.h>

typedef enum e_games {
	SELECT_GAME,
	SNAKE,
	TETRIS
} t_game;

t_game selectedGame;

#endif /* GAMESELECT_H_ */