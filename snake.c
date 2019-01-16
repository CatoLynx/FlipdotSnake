/*
 * snake.c
 *
 * Created: 09.01.2019 20:40:19
 *  Author: Julian
 */ 

#include "snake.h"
#include "graphics.h"
#include "flipdot.h"
#include <stdlib.h>

unsigned int playfield[MATRIX_WIDTH] = {0};
unsigned int oldPlayfield[MATRIX_WIDTH] = {0};
uint8_t snakeVertices[NUM_VERTICES*2] = {0}; // 2 bytes per vertex
unsigned int curSnakeVertex = 0;
int curXPos = 0;
int curYPos = 0;
uint8_t curFoodXPos = 0;
uint8_t curFoodYPos = 0;
uint8_t foodEaten = 1;
unsigned int curSnakeLength = INITIAL_SNAKE_LENGTH;
int curInterval = 500;
t_direction newDirection = INVALID;
t_direction curDirection = INVALID;
t_direction lastDirection = INVALID;

uint8_t getRandomNumber(uint8_t min, uint8_t max) {
	return ((unsigned long)rand() * (max - min) / RAND_MAX) + min;
}

t_direction getDPad() {
	unsigned int adcResult = ADCL;
	adcResult |= ((unsigned int)ADCH << 8);
	if(adcResult < 20) return UP;
	if(adcResult > 300 && adcResult < 350) return DOWN;
	if(adcResult > 490 && adcResult < 530) return LEFT;
	if(adcResult > 830 && adcResult < 870) return RIGHT;
	return INVALID;
}

void updateDirection() {
	newDirection = getDPad();
	if(newDirection != INVALID) curDirection = newDirection;
}

void generateObjects() {
	// Generate food
	if(foodEaten) {
		for(uint8_t i = 0; i < 10; i++) {
			// Try 10 times to avoid putting the food in the snake
			curFoodXPos = getRandomNumber(0, MATRIX_WIDTH - 1);
			curFoodYPos = getRandomNumber(0, VIEWPORT_HEIGHT - 1);
			if(checkForSnake(curFoodXPos, curFoodYPos) == 0) break;
		}
		foodEaten = 0;
	}
	setPixelAt(&playfield, curFoodXPos, curFoodYPos, 1);
}

void clearPlayfield() {
	memset(&playfield, 0, MATRIX_WIDTH*2);
}

void outputPlayfield() {
	setMatrix(&playfield, &oldPlayfield);
	memcpy(&oldPlayfield, &playfield, MATRIX_WIDTH*2);
	unsigned int count = curInterval;
	while(count--) {
		updateDirection();
		_delay_ms(1);

	}
}

void endGame() {
	clearPlayfield();
	drawRectangle(&playfield, 0, 0, MATRIX_WIDTH - 1, VIEWPORT_HEIGHT - 1, 1);
	drawLine(&playfield, 0, 0, MATRIX_WIDTH - 1, VIEWPORT_HEIGHT - 1, 1);
	drawLine(&playfield, 0, VIEWPORT_HEIGHT - 1, MATRIX_WIDTH - 1, 0, 1);
	outputPlayfield();
	_delay_ms(1000);
	clearPlayfield();
	drawNumber(&playfield, 0, 0, curSnakeLength, 1);
	outputPlayfield();
}

uint8_t addSnakeVertex(uint8_t x, uint8_t y) {
	// Adds a snake vertex and returns 0 if the snake is full, 1 otherwise
	snakeVertices[curSnakeVertex*2] = x;
	snakeVertices[curSnakeVertex*2+1] = y;
	curSnakeVertex++;
	return (curSnakeVertex < NUM_VERTICES);
}

void resetSnake() {
	// Initialise variables
	curSnakeVertex = 0;
	curXPos = 0;
	curYPos = 0;
	curSnakeLength = INITIAL_SNAKE_LENGTH;
	curInterval = 500;
	
	// Clear the vertex buffer
	memset(snakeVertices, 255, NUM_VERTICES*2);
	
	// Choose initial snake orientation
	curDirection = getRandomNumber(0, 3);
	lastDirection = curDirection;
	
	// Choose snake start point, considering the length and orientation constraints
	switch(curDirection) {
		case UP:
			curXPos = getRandomNumber(0, MATRIX_WIDTH - 1);
			curYPos = getRandomNumber(0, VIEWPORT_HEIGHT - curSnakeLength);
			addSnakeVertex(curXPos, curYPos + (curSnakeLength - 1));
			break;
		
		case DOWN:
			curXPos = getRandomNumber(0, MATRIX_WIDTH - 1);
			curYPos = getRandomNumber(curSnakeLength - 1, VIEWPORT_HEIGHT - 1);
			addSnakeVertex(curXPos, curYPos - (curSnakeLength - 1));
			break;
		
		case LEFT:
			curXPos = getRandomNumber(0, MATRIX_WIDTH - curSnakeLength);
			curYPos = getRandomNumber(0, VIEWPORT_HEIGHT - 1);
			addSnakeVertex(curXPos + (curSnakeLength - 1), curYPos);
			break;
		
		case RIGHT:
			curXPos = getRandomNumber(curSnakeLength - 1, MATRIX_WIDTH - 1);
			curYPos = getRandomNumber(0, VIEWPORT_HEIGHT - 1);
			addSnakeVertex(curXPos - (curSnakeLength - 1), curYPos);
			break;
	}
}

void renderSnake() {
	uint8_t x0, y0, x1, y1;
	unsigned int dist = 0;
	unsigned int totalDist = 1;
	t_direction dir = INVALID;
	for(int i = NUM_VERTICES - 2; i >= 0; i--) {
		// Render the snake from the newest vertex to the oldest so we can stop if the length is reached
		x0 = snakeVertices[i*2+2];
		y0 = snakeVertices[i*2+3];
		x1 = snakeVertices[i*2];
		y1 = snakeVertices[i*2+1];
		if(x1 == 255 && y1 == 255) continue;
		if(x0 == 255 && y0 == 255 && x1 != 255 && y1 != 255) {
			// First point is the current point
			x0 = curXPos;
			y0 = curYPos;
		}
		dist = getSnakeDistance(x0, y0, x1, y1);
		totalDist += dist;
		if(totalDist >= curSnakeLength) {
			dir = getSnakeDirection(x0, y0, x1, y1);
			uint8_t diff = totalDist - curSnakeLength;
			if(dir == UP) {
				y1 += diff;
			} else if(dir == DOWN) {
				y1 -= diff;
			} else if(dir == LEFT) {
				x1 += diff;
			} else if(dir == RIGHT) {
				x1 -= diff;
			}
			drawLine(&playfield, x0, y0, x1, y1, 1);
			break;
		} else {
			drawLine(&playfield, x0, y0, x1, y1, 1);
		}
	}
}

t_collisionType advanceSnake() {
	// Advances the snake by one step and adds a vertex if the direction has changed
	// Returns the type of collision, if any
	updateDirection();
	if(curDirection == INVALID ||
	   curDirection == UP && lastDirection == DOWN ||
	   curDirection == DOWN && lastDirection == UP ||
	   curDirection == LEFT && lastDirection == RIGHT ||
	   curDirection == RIGHT && lastDirection == LEFT) {
		curDirection = lastDirection;
	}
	if(curDirection != lastDirection) {
		if(addSnakeVertex(curXPos, curYPos) == 0) return LENGTH;
	}
	lastDirection = curDirection;
	
	switch(curDirection) {
		case UP:
			if(checkForSnake(curXPos, curYPos - 1)) return SELF;
			curYPos--;
			break;
		
		case DOWN:
			if(checkForSnake(curXPos, curYPos + 1)) return SELF;
			curYPos++;
			break;
		
		case LEFT:
			if(checkForSnake(curXPos - 1, curYPos)) return SELF;
			curXPos--;
			break;
		
		case RIGHT:
			if(checkForSnake(curXPos + 1, curYPos)) return SELF;
			curXPos++;
			break;
	}
	
	if(curXPos < 0 || curXPos >= MATRIX_WIDTH || curYPos < 0 || curYPos >= VIEWPORT_HEIGHT) return OUTOFBOUNDS;
	if(checkForWall(curXPos, curYPos)) return WALL;
	if(curXPos == curFoodXPos && curYPos == curFoodYPos) {
		feedSnake(1);
		foodEaten = 1;
	}
	return NONE;
}

void feedSnake(uint8_t amount) {
	curSnakeLength += amount;
	curInterval = 500 - (curSnakeLength / 10) * 50;
	if(curInterval < 100) curInterval = 100;
}

unsigned int getSnakeDistance(int x0, int y0, int x1, int y1) {
	// Calculates the distance between two points around a corner (not straight)
	return abs(x1 - x0) + abs(y1 - y0);
}

t_direction getSnakeDirection(int x0, int y0, int x1, int y1) {
	// Calculates the direction the snake has to travel between two points
	// The points must not be diagonally apart
	if(y1 < y0) return UP;
	if(y1 > y0) return DOWN;
	if(x1 < x0) return LEFT;
	if(x1 > x0) return RIGHT;
	return INVALID;
}

uint8_t checkForWall(int x, int y) {
	return 0;
}

uint8_t checkForSnake(int x, int y) {
	// Checks if the given point lies within the snake
	uint8_t x0, y0, x1, y1;
	unsigned int dist = 0;
	unsigned int totalDist = 1;
	t_direction dir = INVALID;
	for(int i = NUM_VERTICES - 2; i >= 0; i--) {
		// Check the snake from the newest vertex to the oldest so we can stop if the length is reached
		x0 = snakeVertices[i*2+2];
		y0 = snakeVertices[i*2+3];
		x1 = snakeVertices[i*2];
		y1 = snakeVertices[i*2+1];
		if(x1 == 255 && y1 == 255) continue;
		if(x0 == 255 && y0 == 255 && x1 != 255 && y1 != 255) {
			// First point is the current point
			x0 = curXPos;
			y0 = curYPos;
		}
		dist = getSnakeDistance(x0, y0, x1, y1);
		totalDist += dist;
		if(totalDist >= curSnakeLength) {
			dir = getSnakeDirection(x0, y0, x1, y1);
			uint8_t diff = totalDist - curSnakeLength;
			if(dir == UP) {
				y1 += diff;
				} else if(dir == DOWN) {
				y1 -= diff;
				} else if(dir == LEFT) {
				x1 += diff;
				} else if(dir == RIGHT) {
				x1 -= diff;
			}
			if(isPointOnLine(x, y, x0, y0, x1, y1)) return 1;
			break;
		} else {
			if(isPointOnLine(x, y, x0, y0, x1, y1)) return 1;
		}
	}
	return 0;
}