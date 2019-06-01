/*
 * tetris.h
 *
 * Created: 30.05.2019 15:29:33
 *  Author: Julian
 */ 


#ifndef TETRIS_H_
#define TETRIS_H_

#include <stdint.h>
#include "game.h"
#include "graphics.h"

#define NUM_BLOCK_TYPES 7
#define START_FALL_INTERVAL 250
#define QUICK_FALL_INTERVAL 50
#define FALL_INTERVAL_PER_LEVEL 50
#define LINES_PER_LEVEL 10
#define MAX_LEVEL 4
#define LINE_CHECK_MASK 0b1111111111111000

#define EEPROM_ADDR_TETRIS_HIGH_SCORE 0x100

typedef enum e_blockTypes {
	BLOCK_I,
	BLOCK_O,
	BLOCK_L,
	BLOCK_J,
	BLOCK_S,
	BLOCK_Z,
	BLOCK_T
} t_blockType;

typedef enum e_blockRotations {
	ROT_90,
	ROT_180,
	ROT_270,
	ROT_0,
} t_blockRotation;

typedef struct s_blockData {
	t_blockType type;
	t_blockRotation rotation;
	int16_t x;
	int16_t y;
} t_blockData;

t_blockData placedBlocks[200];

void outputTetrisPlayfield();
t_collision drawBlock(uint16_t* buf, t_blockType blockType, t_blockRotation rotation, int16_t x0, int16_t y0, uint8_t state);
t_collision drawBlockData(uint16_t* buf, t_blockData block, uint8_t state);
void generateNewBlock();
void placeCurrentBlock();
uint8_t checkLineFull(uint8_t x);
void removeLine(uint8_t x);
void removeFullLines();
void tetrisGameOver();
void updateTetrisHighScore();
void tetrisInit();
void tetrisLoop();

#endif /* TETRIS_H_ */