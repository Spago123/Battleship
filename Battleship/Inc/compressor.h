/**
 * @file compressing.h
 * @author Harun Špago (hspago1@etf.unsa.ba)
 * @brief Compressor tool for matrices with dimensions NxN (further on boards) that have four possible states for each field
 *        This compressor tool reduces the memory that the board occupies by four
 *        Ex. for a board that has 10x10 = 100 filed(bayts) this tool gives us the abilit to store all that data,
 *        without lossing anything in 25 bayts, as char array (string)
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef COMPRESSING_H
#define COMPRESSING_H

#include "main.h"
#include <stdbool.h>

///BOARD SIZE
#define SIZE 10

///@brief POSSIBLE BOARD FIELD STATES
#define EMPTY 0x00 /// ' '
#define BOAT 0x01 /// 'o'
#define MISS 0x02 /// 'x'
#define SINK 0x03 /// '+'

#define EMPTY_FIELD ' '
#define BOAT_FIELD 'o'
#define MISS_FIELD 'x'
#define SINK_FIELD '+'

#define SET_BIT_COMPRESSOR(arr, bit_position) ((arr[(bit_position) / 8] |= (1 << ((bit_position) % 8))))
#define RESET_BIT_COMPRESSOR(arr, bit_position) ((arr[(bit_position) / 8] &= ~(1 << ((bit_position) % 8))))
#define READ_BIT_COMPRESSOR(arr, bit_position) ((arr[(bit_position) / 8] >> ((bit_position) % 8) & 1))

/**
 * @brief Function that takes a boards an compresses it into a string
 * 
 * @param board, NxN matrix
 * @return unsigned* char, string with the compressed board
 * 
 * @note code can easily be written to work for double pointers, unsigned char**
 */
void compressBoard(uint8_t board[][SIZE], uint8_t *strBoard, const bool hideShips);
/**
 * @brief Function that decompresses data from a string to the original boards of NxN 
 * 
 * @param stringBoard, string that contains all the data necessary for board rebuild
 * @return unsigned**, board that contains all the data needed
 */
void decompressBoard(uint8_t board[][SIZE], uint8_t *strBoard);

#endif