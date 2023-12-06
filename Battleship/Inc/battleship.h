/**
 * @file battleship.h
 * @author Harun špago (hspago1@etf.unsa.ba)
 * @brief Library used to play battleship
 * @note to play this game you need two stm32f3 boards
 *       and you have to connect those boards with two PC-s
 *       also you have to connect those boards in the following way:
 *       -----------------------------
 *       | Board 1 pin | Board 2 pin | 
 *       |     PB1     |     PB2     |
 *       |     PB2     |     PB1     |
 *       |     PA2     |     PA3     |
 *       |     PA3     |     PA2     |
 *       -----------------------------
 * @version 0.1
 * @date 2023-11-07
 * 
 * @note USART2 is used for communication between boards,
 *       while USART1 is used for communication between board and PC
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "main.h"
#include "battleship_crc.h"
#include "connector.h"
#include "compressor.h"
#include "button.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "parser.h"


#define SIZE 10
#define MAX_MSG 30


#define BOARD_A 'A'
#define BOARD_B 'B'

/// @brief PrivState is used to sync the communication between the two boards, even in the phase of the game where 
/// it hasn't been decided who is boardA and boardB
typedef enum{
	BOSS_LINE, 
	WORKER_LINE
}PrivState;


/// @brief Phases of the game
typedef enum{
    STEADY, 
		PINGINIG_FOR_GAME,
    FOUND_GAME,
		TRANSIT_STATE,
    SETTING_UP_THE_BOARD,
    EXCHANGE_BOARDS,
    PLAYING_GAME
} GamePhase;

/// @brief TurnPhase is used to descriebe various phases in which the two stm board can be during the PLAYING_GAME
typedef enum{
    USER_INPUT, 
    PARSING,
    SENDING,
    RECEIVING,
    OK_CONFIRMATION
} TurnPhase;

/// @brief GamePhase is used to exchange the data of the board between the stm boards
typedef enum{
	UNDEFINED,/// It hasn't been decided which board goes first

	PLAYING_MODE_A, /// This mode is described with the following, this board is waiting user input
    /// from PC, so it is receiving that input, than it is sending an error message depending on the given message
    /// after that it is sending formatted data to the other board and than we go into WAITING_MODE_B

  INCOMING, 

	WAITING_MODE_B, /// This mode is described with the following, this board is waiting for a message from the other board, 
    /// Than it checks the message, and the CRC, if something goes wrong it send an adequate error message to the other board
    ///and then it goes into PLAYING_MODE_A

  CRC_CHECK
}GameSubPhase;

/// @brief just type this message into you main fajl and have fun playing the game
void playTheGame();
/**
 * @brief This funcrion is used to pack the board name, board data, and crc code in the following form
 * <boardName>:<compressed board data 1 - 26><crcCode 27 - 28> 
 * 
 * @param message message to be sent to the other board
 * @param boardName A or B
 * @param board board data in the format of a matrix
 */
void packBoardMsg(uint8_t message[MAX_MSG], uint8_t boardName, uint8_t board[][SIZE], bool hideShips);
/// @brief Function used to parse the user input
/// @param message message received from the pc 
/// @return 
ErrorCodes parseUserInput(uint8_t message[]);
/// @brief Function used to parse the message recieved from the other stm board
/// @param message received from th eother board
/// @return 
ErrorCodes parseBoardInput(uint8_t message[]);

/// @brief function used to handle the communication between the board and the pc
bool stateMachinePC(void);
/// @brief function used to handle to communication between boards 
bool stateMachineBoardComm(void);

///void packBoardsToBeSentToPC();

#endif