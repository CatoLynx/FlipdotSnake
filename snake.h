/*
 * snake.h
 *
 * Created: 09.01.2019 20:40:31
 *  Author: Julian
 */ 


#ifndef SNAKE_H_
#define SNAKE_H_

#include <stdint.h>

#define NUM_VERTICES 256
#define INITIAL_SNAKE_LENGTH 5

typedef enum e_directions {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	INVALID
} t_direction;

typedef enum e_collisionTypes {
	NONE,
	OUTOFBOUNDS,
	WALL,
	SELF,
	LENGTH
} t_collisionType;

uint8_t getRandomNumber(uint8_t min, uint8_t max);
t_direction getDPad();
void updateDirection();
void generateObjects();
void clearPlayfield();
void outputPlayfield();
void endGame();
uint8_t addSnakeVertex(uint8_t x, uint8_t y);
void resetSnake();
void renderSnake();
t_collisionType advanceSnake();
void feedSnake(uint8_t amount);
unsigned int getSnakeDistance(int x0, int y0, int x1, int y1);
t_direction getSnakeDirection(int x0, int y0, int x1, int y1);
uint8_t checkForWall(int x, int y);
uint8_t checkForSnake(int x, int y);

#endif /* SNAKE_H_ */