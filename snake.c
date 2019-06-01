/*
 * snake.c
 *
 * Created: 09.01.2019 20:40:19
 *  Author: Julian
 */ 

#include "snake.h"
#include "game.h"
#include "graphics.h"
#include "flipdot.h"
#include <stdlib.h>

#include <avr/eeprom.h>

uint8_t snakeVertices[NUM_VERTICES*2] = {0}; // 2 bytes per vertex
int16_t nextFreeSnakeVertex = 0;
uint16_t numUsedVertices = 0;
int16_t curXPos = 0;
int16_t curYPos = 0;
uint8_t curFoodXPos = 0;
uint8_t curFoodYPos = 0;
uint8_t foodEaten = 1;
uint16_t curSnakeLength = INITIAL_SNAKE_LENGTH;
int16_t curInterval = START_INTERVAL;
t_direction newDirection = INVALID;
t_direction curDirection = INVALID;
t_direction lastDirection = INVALID;
t_collisionType collisionType = NONE;
uint32_t snakeScore = 0;

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

void outputSnakePlayfield() {
	outputPlayfield();
	uint16_t count = curInterval;
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
	updateSnakeHighScore();
	uint32_t highScore = eeprom_read_dword(EEPROM_ADDR_SNAKE_HIGH_SCORE);
	drawNumber(&playfield, 0, 0, snakeScore, 1);
	drawNumber(&playfield, 0, 8, highScore, 1);
	outputPlayfield();
}

uint8_t addSnakeVertex(uint8_t x, uint8_t y) {
	// Tries to add a snake vertex and returns 0 if the snake is full (failure), 1 on success
	if(numUsedVertices >= NUM_VERTICES) return 0;
	snakeVertices[nextFreeSnakeVertex*2] = x;
	snakeVertices[nextFreeSnakeVertex*2+1] = y;
	nextFreeSnakeVertex++;
	nextFreeSnakeVertex %= NUM_VERTICES;
	numUsedVertices++;
	return 1;
}

uint8_t getSnakeVertexX(int16_t index) {
	while (index < 0) index += NUM_VERTICES;
	index %= NUM_VERTICES;
	return snakeVertices[index*2];
}

uint8_t getSnakeVertexY(int16_t index) {
	while (index < 0) index += NUM_VERTICES;
	index %= NUM_VERTICES;
	return snakeVertices[index*2+1];
}

void resetSnake() {
	// Initialise variables
	nextFreeSnakeVertex = 0;
	curXPos = 0;
	curYPos = 0;
	curSnakeLength = INITIAL_SNAKE_LENGTH;
	curInterval = START_INTERVAL;
	
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
	uint16_t dist = 0;
	uint16_t totalDist = 1;
	t_direction dir = INVALID;
	int16_t i = 0;
	uint16_t vertexCount = 0;
	
	// TODO: STILL BUGGY WITH SMALL RINGBUFFER
	
	for(int16_t j = 0; j < NUM_VERTICES; j++) {
		// Render the snake from the newest vertex to the oldest so we can stop if the length is reached
		i = (nextFreeSnakeVertex - j);
		if(i < 0) i += NUM_VERTICES;
		
		x0 = getSnakeVertexX(i+1);
		y0 = getSnakeVertexY(i+1);
		x1 = getSnakeVertexX(i);
		y1 = getSnakeVertexY(i);
		if(x1 == 255 && y1 == 255) continue;
		if(x0 == 255 && y0 == 255 && x1 != 255 && y1 != 255) {
			// First point16_t is the current point
			x0 = curXPos;
			y0 = curYPos;
		}
		
		dist = getSnakeDistance(x0, y0, x1, y1);
		totalDist += dist;
		vertexCount++;
		
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
	
	numUsedVertices = vertexCount + 1; // +1 as a reserve in case the snake gets fed and the last vertex has to be reused
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
	snakeScore = curSnakeLength - INITIAL_SNAKE_LENGTH;
	curInterval = START_INTERVAL - (curSnakeLength / BLOCKS_PER_SPEEDUP) * SPEEDUP_FACTOR;
	if(curInterval < MIN_INTERVAL) curInterval = MIN_INTERVAL;
}

uint16_t getSnakeDistance(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
	// Calculates the distance between two points around a corner (not straight)
	return abs(x1 - x0) + abs(y1 - y0);
}

t_direction getSnakeDirection(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
	// Calculates the direction the snake has to travel between two points
	// The points must not be diagonally apart
	if(y1 < y0) return UP;
	if(y1 > y0) return DOWN;
	if(x1 < x0) return LEFT;
	if(x1 > x0) return RIGHT;
	return INVALID;
}

uint8_t checkForWall(int16_t x, int16_t y) {
	return 0;
}

uint8_t checkForSnake(int16_t x, int16_t y) {
	// Checks if the given point16_t lies within the snake
	uint8_t x0, y0, x1, y1;
	uint16_t dist = 0;
	uint16_t totalDist = 1;
	t_direction dir = INVALID;
	int16_t i = 0;
	
	for(int16_t j = 0; j < NUM_VERTICES; j++) {
		// Check the snake from the newest vertex to the oldest so we can stop if the length is reached
		i = (nextFreeSnakeVertex - j);
		if(i < 0) i += NUM_VERTICES;
		
		x0 = getSnakeVertexX(i+1);
		y0 = getSnakeVertexY(i+1);
		x1 = getSnakeVertexX(i);
		y1 = getSnakeVertexY(i);
		if(x1 == 255 && y1 == 255) continue;
		if(x0 == 255 && y0 == 255 && x1 != 255 && y1 != 255) {
			// First point16_t is the current point
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

void updateSnakeHighScore() {
	uint32_t highScore = eeprom_read_dword(EEPROM_ADDR_SNAKE_HIGH_SCORE);
	if(highScore == (uint32_t)-1) highScore = 0;
	if(snakeScore > highScore) eeprom_update_dword(EEPROM_ADDR_SNAKE_HIGH_SCORE, snakeScore);
}

void snakeInit() {
	//nothing to do
}

void snakeLoop() {
	while(1) {
		clearPlayfield();
		resetSnake();
		clearPlayfield();
		renderSnake();
		outputSnakePlayfield();
		while(getDPad() != INVALID);
		while(getDPad() == INVALID);
	
		while (1)
		{
			clearPlayfield();
			generateObjects();
			collisionType = advanceSnake();
			if(collisionType != NONE) {
				endGame();
				return;
			}
			renderSnake();
			outputSnakePlayfield();
		}
	}
}