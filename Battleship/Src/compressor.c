#include "compressor.h"



void compressBoard(uint8_t board[][SIZE], uint8_t *strBoard, const bool hideShips){
    uint8_t i, j;
    for(i = 0, j = 0; i < SIZE * SIZE; i+=1, j+=2){
        if(board[i/SIZE][i%SIZE] == EMPTY_FIELD ||
						(board[i/SIZE][i%SIZE] == BOAT_FIELD && hideShips)){//EMPTY
            RESET_BIT_COMPRESSOR(strBoard, j);
            RESET_BIT_COMPRESSOR(strBoard, (j + 1));            
        }else if(board[i/SIZE][i%SIZE] == BOAT_FIELD && !hideShips){//BOAT
            SET_BIT_COMPRESSOR(strBoard, j);
            RESET_BIT_COMPRESSOR(strBoard, (j + 1));                    
        }else if(board[i/SIZE][i%SIZE] == MISS_FIELD){//MISS
            RESET_BIT_COMPRESSOR(strBoard, j);
            SET_BIT_COMPRESSOR(strBoard, (j + 1));           
        }else if(board[i/SIZE][i%SIZE] == SINK_FIELD){//SINK
            SET_BIT_COMPRESSOR(strBoard, j);
            SET_BIT_COMPRESSOR(strBoard, (j + 1));                
        }
    }
}

void decompressBoard(uint8_t board[][SIZE], uint8_t *strBoard){
		uint8_t i, j;
    for(i = 0, j = 0; i < SIZE * SIZE; i+=1, j+=2){
        uint8_t field = READ_BIT_COMPRESSOR(strBoard, (j + 1));  // Read the value at position j + 1
        field <<= 1;  // Shift the rightmost bit one position to the left
        field |= READ_BIT_COMPRESSOR(strBoard, (j));  // Perform a bitwise OR with the previous bit

        if(field == EMPTY){
            board[i/SIZE][i%SIZE] = EMPTY_FIELD;
        }else if(field == BOAT){
            board[i/SIZE][i%SIZE] = BOAT_FIELD;
        }else if(field == MISS){
            board[i/SIZE][i%SIZE] = MISS_FIELD;
        }else if(field == SINK){
            board[i/SIZE][i%SIZE] = SINK_FIELD;
        }
    }

}