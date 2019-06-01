/*
 * snake.h
 *
 * Created: 09.01.2019 20:40:31
 *  Author: Julian
 */ 


#ifndef SNAKE_H_
#define SNAKE_H_

#include <stdint.h>
#include "game.h"

#define NUM_VERTICES 256
#define INITIAL_SNAKE_LENGTH 5
#define START_INTERVAL 150
#define MIN_INTERVAL 50
#define BLOCKS_PER_SPEEDUP 5
#define SPEEDUP_FACTOR 10

#define EEPROM_ADDR_SNAKE_HIGH_SCORE 0x050

typedef enum e_collisionTypes {
	NONE,
	OUTOFBOUNDS,
	WALL,
	SELF,
	LENGTH
} t_collisionType;

void updateDirection();
void generateObjects();
void outputSnakePlayfield();
void endGame();
uint8_t addSnakeVertex(uint8_t x, uint8_t y);
uint8_t getSnakeVertexX(int16_t index);
uint8_t getSnakeVertexY(int16_t index);
void resetSnake();
void renderSnake();
t_collisionType advanceSnake();
void feedSnake(uint8_t amount);
uint16_t getSnakeDistance(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
t_direction getSnakeDirection(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
uint8_t checkForWall(int16_t x, int16_t y);
uint8_t checkForSnake(int16_t x, int16_t y);
void updateSnakeHighScore();
void snakeInit();
void snakeLoop();

#endif /* SNAKE_H_ */