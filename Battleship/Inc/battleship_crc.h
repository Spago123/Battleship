/**
 * @file battleship_crc.h
 * @author Harun Špago (hspago1@etf.unsa.ba)
 * @brief CRC code generator for battleship game. This crc code occupies
 *        2 bytes of memory
 * @version 0.1
 * @date 2023-11-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef BATTLESHIP_CRC_H
#define BATTLESHIP_CRC_H

#include <stdbool.h>
#include "main.h"
#include <string.h>

#define POLYNOMIAL 0x1021
#define POLYNOMIAL_WIDTH 16
#define CRC_SIZE 2
#define MSG_SIZE 29
typedef union{
    uint16_t crc;
    uint8_t crcBy8[2];
}CrcCode;

/**
 * @brief function used to calcaute the crc code
 * 
 * @param data foe which you are calculating the crc
 * @return uint16_t 
 */
uint16_t calculateCrc(const uint8_t *data);
/**
 * @brief Function used to add the crc code to the end of the data that has to be transmitted
 * 
 * @param data 
 */
void appendCrcCode(uint8_t *data);
/**
 * @brief Function used to check the crc code
 * 
 * @param msg 
 * @return true if crc is okey
 * @return false if the crc does not match
 */
bool checkCrc(uint8_t *msg);
/**
 * @brief Function used to delete the crc code from data
 * 
 * @param data 
 */
void eraseCrcCode(uint8_t data[]);

#endif