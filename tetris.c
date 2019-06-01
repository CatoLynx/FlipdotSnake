/*
 * tetris.c
 *
 * Created: 30.05.2019 15:31:31
 *  Author: Julian
 */ 

#include "main.h"
#include "tetris.h"
#include "flipdot.h"
#include "graphics.h"

const uint8_t blockI[4*4] = {
	0b0001, 0b0001, 0b0001, 0b0001,	// Rotation 90°
	0b0000, 0b0000, 0b0000, 0b1111,	// Rotation 180°
	0b0001, 0b0001, 0b0001, 0b0001,	// Rotation 270°
	0b0000, 0b0000, 0b0000, 0b1111,	// Rotation 0°
};

const uint8_t blockO[4*4] = {
	0b0000, 0b0000, 0b0011, 0b0011,
	0b0000, 0b0000, 0b0011, 0b0011,
	0b0000, 0b0000, 0b0011, 0b0011,
	0b0000, 0b0000, 0b0011, 0b0011
};

const uint8_t blockL[4*4] = {
	0b0000, 0b0011, 0b0001, 0b0001,
	0b0000, 0b0000, 0b0001, 0b0111,
	0b0000, 0b0010, 0b0010, 0b0011,
	0b0000, 0b0000, 0b0111, 0b0100,
};

const uint8_t blockJ[4*4] = {
	0b0000, 0b0001, 0b0001, 0b0011,
	0b0000, 0b0000, 0b0100, 0b0111,
	0b0000, 0b0011, 0b0010, 0b0010,
	0b0000, 0b0000, 0b0111, 0b0001
};

const uint8_t blockS[4*4] = {
	0b0000, 0b0010, 0b0011, 0b0001,
	0b0000, 0b0000, 0b0011, 0b0110,
	0b0000, 0b0010, 0b0011, 0b0001,
	0b0000, 0b0000, 0b0011, 0b0110
};

const uint8_t blockZ[4*4] = {
	0b0000, 0b0001, 0b0011, 0b0010,
	0b0000, 0b0000, 0b0110, 0b0011,
	0b0000, 0b0001, 0b0011, 0b0010,
	0b0000, 0b0000, 0b0110, 0b0011
};

const uint8_t blockT[4*4] = {
	0b0000, 0b0001, 0b0011, 0b0001,
	0b0000, 0b0000, 0b0010, 0b0111,
	0b0000, 0b0010, 0b0011, 0b0010,
	0b0000, 0b0000, 0b0111, 0b0010
};

t_blockData currentBlock = {
	.type = BLOCK_T,
	.rotation = ROT_90,
	.x = MATRIX_WIDTH,
	.y = VIEWPORT_HEIGHT / 2
};

uint8_t buttonPressed = 0;
unsigned long long lastBlockAdvance = 0;
unsigned int placedBlocksPlayfield[MATRIX_WIDTH] = {0};
unsigned int fallInterval = START_FALL_INTERVAL;
unsigned int oldFallInterval = START_FALL_INTERVAL;
uint8_t quickDropActive = 0;
uint8_t firstMoveOfNewBlock = 1;
unsigned int score = 0;
uint8_t level = 0;
unsigned int numLines = 0;

t_collision drawBlock(unsigned int* buf, t_blockType blockType, t_blockRotation rotation, int x0, int y0, uint8_t state) {
	uint8_t ret = 1;
	uint8_t* blockBitmap;
	uint8_t xOffset, yOffset = 0;
	switch(blockType) {
		case BLOCK_I: {
			switch(rotation) {
				case ROT_0:
				case ROT_180: {
					xOffset = 1;
					yOffset = 3;
					break;
				}
				case ROT_90:
				case ROT_270: {
					xOffset = 3;
					yOffset = 2;
					break;
				}
			}
			blockBitmap = &blockI[rotation*4];
			break;
		}
		case BLOCK_O: {
			xOffset = 2;
			yOffset = 2;
			blockBitmap = &blockO[rotation*4];
			break;
		}
		case BLOCK_L: {
			switch(rotation) {
				case ROT_0:
				case ROT_270: {
					xOffset = 1;
					yOffset = 2;
					break;
				}
				case ROT_90: {
					xOffset = 2;
					yOffset = 2;
					break;
				}
				case ROT_180: {
					xOffset = 1;
					yOffset = 3;
					break;
				}
			}
			blockBitmap = &blockL[rotation*4];
			break;
		}
		case BLOCK_J: {
			switch(rotation) {
				case ROT_0:
				case ROT_270: {
					xOffset = 1;
					yOffset = 2;
					break;
				}
				case ROT_90: {
					xOffset = 2;
					yOffset = 2;
					break;
				}
				case ROT_180: {
					xOffset = 1;
					yOffset = 3;
					break;
				}
			}
			blockBitmap = &blockJ[rotation*4];
			break;
		}
		case BLOCK_S: {
			xOffset = 2;
			yOffset = 2;
			blockBitmap = &blockS[rotation*4];
			break;
		}
		case BLOCK_Z: {
			switch(rotation) {
				case ROT_0:
				case ROT_180: {
					xOffset = 1;
					yOffset = 2;
					break;
				}
				case ROT_90:
				case ROT_270: {
					xOffset = 2;
					yOffset = 2;
					break;
				}
			}
			blockBitmap = &blockZ[rotation*4];
			break;
		}
		case BLOCK_T: {
			switch(rotation) {
				case ROT_0:
				case ROT_270: {
					xOffset = 1;
					yOffset = 2;
					break;
				}
				case ROT_90: {
					xOffset = 2;
					yOffset = 2;
					break;
				}
				case ROT_180: {
					xOffset = 1;
					yOffset = 3;
					break;
				}
			}
			blockBitmap = &blockT[rotation*4];
			break;
		}
	}
	return drawBitmapRowMajor(buf, x0-xOffset, y0-yOffset, 4, 4, blockBitmap, state, 1);
}

t_collision drawBlockData(unsigned int* buf, t_blockData block, uint8_t state) {
	return drawBlock(buf, block.type, block.rotation, block.x, block.y, state);
}

void generateNewBlock() {
	t_blockType type = getRandomNumber(0, NUM_BLOCK_TYPES);
	currentBlock.type = type;
	currentBlock.rotation = ROT_90;
	currentBlock.x = MATRIX_WIDTH - 2;
	currentBlock.y = VIEWPORT_HEIGHT / 2;
	firstMoveOfNewBlock = 1;
}

void placeCurrentBlock() {
	drawBlockData(&placedBlocksPlayfield, currentBlock, 1);
}

uint8_t checkLineFull(uint8_t x) {
	return ((placedBlocksPlayfield[x] & LINE_CHECK_MASK) == LINE_CHECK_MASK);
}

void removeLine(uint8_t x) {
	placedBlocksPlayfield[x] &= ~LINE_CHECK_MASK;
	for(uint8_t i = x; i < MATRIX_WIDTH-1; i++) {
		placedBlocksPlayfield[i] = placedBlocksPlayfield[i+1];
	}
	placedBlocksPlayfield[MATRIX_WIDTH-1] &= ~LINE_CHECK_MASK;
	score += 40 * (level + 1);
	numLines++;
	if(numLines % 10 == 0) level++;
	if(level > 10) level = 10;
	fallInterval = START_FALL_INTERVAL - (level*FALL_INTERVAL_PER_LEVEL);
}

void removeFullLines() {
	for(uint8_t i = 0; i < MATRIX_WIDTH; i++) {
		if(checkLineFull(i)) {
			removeLine(i);
			i--;
		}
	}
}

void tetrisGameOver() {
	clearPlayfield();
	memset(&placedBlocksPlayfield, 0, MATRIX_WIDTH*2);
	drawRectangle(&playfield, 0, 0, MATRIX_WIDTH - 1, VIEWPORT_HEIGHT - 1, 1);
	drawLine(&playfield, 0, 0, MATRIX_WIDTH - 1, VIEWPORT_HEIGHT - 1, 1);
	drawLine(&playfield, 0, VIEWPORT_HEIGHT - 1, MATRIX_WIDTH - 1, 0, 1);
	outputPlayfield();
	_delay_ms(1000);
	clearPlayfield();
	drawNumber(&playfield, 0, 0, score, 1);
	outputPlayfield();
	score = 0;
	level = 0;
}

void tetrisLoop() {
	uint8_t blockMovedUser = 0;
	uint8_t blockMovedAuto = 0;
	t_direction dir = getDPad();
	if(buttonPressed == 0) {
		switch(dir) {
			case UP: {
				buttonPressed = 1;
				currentBlock.rotation++;
				currentBlock.rotation %= 4;
				blockMovedUser = 1;
				break;
			}
			case DOWN: {
				oldFallInterval = fallInterval;
				fallInterval = QUICK_FALL_INTERVAL;
				quickDropActive = 1;
				buttonPressed = 1;
				break;
			}
			case LEFT: {
				buttonPressed = 1;
				currentBlock.y--;
				blockMovedUser = 1;
				break;
			}
			case RIGHT: {
				buttonPressed = 1;
				currentBlock.y++;
				blockMovedUser = 1;
				break;
			}
		}
	} else {
		if(dir == INVALID) {
			buttonPressed = 0;
			fallInterval = oldFallInterval;
			quickDropActive = 0;
			_delay_ms(100);
		}
	}
	
	if(sysTicks - lastBlockAdvance >= fallInterval) {
		lastBlockAdvance = sysTicks;
		currentBlock.x--;
		blockMovedAuto = 1;
		if(quickDropActive) score ++;
	}
	
	uint8_t blockPlaced = 0;
	if(blockMovedAuto || blockMovedUser) {
		clearPlayfield();
		overlayPlayfield(&placedBlocksPlayfield);
		t_collision collision = drawBlockData(&playfield, currentBlock, 1);
		
		if((collision & PIXEL) || (collision & OOB_LEFT)) {
			if(blockMovedAuto) {
				currentBlock.x++;
				blockPlaced = 1;
			}
		}
		
		if(collision != NO_COLL) {
			// Undo moves if they would result in a collision
			if(blockPlaced && firstMoveOfNewBlock) {
				// Game over
				tetrisGameOver();
				while(getDPad() == INVALID);
				return;
			}
			
			if(!(collision & OOB_RIGHT)) {
				if(blockMovedUser) {
					switch(dir) {
						case UP: {
							currentBlock.rotation--;
							currentBlock.rotation %= 4;
							break;
						}
						case LEFT: {
							currentBlock.y++;
							break;
						}
						case RIGHT: {
							currentBlock.y--;
							break;
						}
					}
				}
			
				restoreOldPlayfield();
			}
		}
		
		if(blockMovedAuto == 1) firstMoveOfNewBlock = 0;
	
		if(blockPlaced) {
			placeCurrentBlock();
			removeFullLines();
			generateNewBlock();
		}
		
		outputPlayfield();
		blockMovedAuto = 0;
		blockMovedUser = 0;
	}
}