/**
 * @file parser.h
 * @author Harun Špago (hspago1@etf.unsa.ba)
 * @brief Library used to parse data comming from the PC
 * @version 0.1
 * @date 2023-11-14
 * 
 * @copyright Copyright (c) 2023
 *
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "compressor.h"
#include "battleship_crc.h"
#include <stdlib.h>
#include <time.h>

#define REQ_RESET_GAME 'G'
#define REQ_RESET_BOARD 'B'
#define REQ_RESET_GAME_YES 'Y'
#define REQ_RESET_GAME_NO 'N'

#define SIZE 10
#define NUM_SHIPS 5

///@brief possible ships
#define NON 'N'
#define CARRIER 'C'
#define BATTLESHIP 'B'
#define DESTROYER 'D'
#define SUBMARINE 'S'
#define PATROL_BOAT 'P'

///@brief possible ship rotations
#define ROTATION_0 0
#define ROTATION_90 1
#define ROTATION_180 2
#define ROTATION_270 3

///@brief Error Codes thet get send from the board to the PC as a sign that something went wrong
typedef enum{
    ERROR_NONE = 0, /// 100
    BOARDS_DO_NOT_MATCH = 1, /// 101
    CRC_CHECK_FAILED = 2, /// 102
    INVALID_INPUT_VIA_TERMINAL = 3, /// 103
    PROPER_GUESS_INVALID_COORDINATES = 4, /// 104
    INVALID_MSG = 5, /// 105
    VICTORY_CHECK_FAILED = 6, /// 106
    RESET_BOARD = 7, /// errors from 7 to 15 are not errors they are just notifications from the code what to do
    RESET_GAME = 8,
		UPD_BOARD = 9,
		GUESS_DETECTED = 10,
		A_MESSAGE = 11,
		B_MESSAGE = 12,
		W_MESSAGE = 13, 
		V_CONFIRMED = 14,
		READY_MSG = 15,
		RESET_GAME_YES = 16,
		RESET_GAME_NO
} ErrorCodes;

/// @brief Message types that the user can type inn via the terminal
typedef enum{
    BOARD_SET_UP = 0,
    PLAYER_GUESS = 1,
    RESET_REQUEST = 2,
		UPDATE_BOARD = 3,
		ECHO_MSG = 4,
		READY = 6
} MessageFromPCType;

/// @brief Message types that the boards are exchanging between themselves
typedef enum{
		PLAYER_GUESS_BOARD = 0,
		BOARD_A_DATA = 1,
		BOARD_B_DATA = 2,
		RESET_REQUEST_BOARD = 3,
		ECHO_MSG_BOARD = 4,   
		WIN_CHECK = 5,
		VICTORY_CONFIRMED = 6,
		OK_CODE = 7,
		ERROR_CODE = 8
} MessageFromBoardType;

/// @brief Object that descriebes a ship
typedef struct {
	uint8_t T; /// type of the ship
	uint8_t Px; /// position alongside the x axis
	uint8_t Py; /// position alongsides the y - ose
	uint8_t R; /// rotation
} ShipObject;

/**
 * @brief Function used to check if an error is bad or just an information 
 * 
 * @param error 
 * @return true 
 * @return false 
 */
bool checkErrorCode(ErrorCodes error);
/**
 * @brief Function used to place ship
 * 
 * @param stringInput user input via CMD from the host PC
 * @param board SIZE X SIZE matrix
 * @return ErrorCodes 
 */
ErrorCodes placeShip2(uint8_t stringInput[], uint8_t board[][SIZE]);
/**
 * @brief Function used to handle user reques, that can either be to reset the game or to reset the board
 * 
 * @param message user input via terminal
 * @param messageToBeCreated message that is going to be created if you are asking for reset of the game
 * @return ErrorCodes 
 */
ErrorCodes handleReq(uint8_t message[], uint8_t messageToBeCreated[]);
/**
 * @brief Function used to handle player's guess
 * 
 * @param message user input via terminal, it should be a message like PGS:Px;Py
 * @param messageToBeCreated message that is going to be send to the other board, format G:Px;Py
 * @param enemyBoard matrix SIZE X SIZE with ships
 * @param leftHits number of hits left on the board
 * @param myTurn indicator if it is my turn to guess (true), either (false)
 * @return ErrorCodes 
 */
ErrorCodes handlePlayerGuess(uint8_t message[], uint8_t messageToBeCreated[], uint8_t enemyBoard[][SIZE], uint8_t *leftHits, bool *myTurn);
/**
 * @brief Function used to handle echo messages, aka message of format ECH:<message> that are send from one terminal directly
 * to the other board via serial comm of the boards
 * 
 * @param message echo message received from the user terminal
 * @param messageToBeCreated message that is created form the echo message format E:<message>
 * @return ErrorCodes 
 */
ErrorCodes handleEchoMessage(uint8_t message[], uint8_t messageToBeCreated[]);
/**
 * @brief Function used to check if a board that has been packed into a string is equal to the board
 * 
 * @param message string where the board is packed
 * @param board original board and it's data
 * @return ErrorCodes 
 */
ErrorCodes checkIfBoardsMatch(uint8_t message[], uint8_t board[][SIZE]);
/**
 * @brief Function that is handling the request message received from the enemy board
 *  
 * @param message received message from the enemies board
 * @param mesageToBeCreated message that is gonna be created according players response
 * @return ErrorCodes 
 */
ErrorCodes handleReqBoard(uint8_t message[], uint8_t mesageToBeCreated[]);
/**
 * @brief Function used to handle errors that are comming from the other board
 * 
 * @param message error message received from the other board
 * @param messageToBecreated place to store the error message, that is going to be send to PC
 * @return ErrorCodes 
 */
ErrorCodes handleErrorFromBoard(uint8_t message[], uint8_t messageToBeCreated[]);
/**
 * @brief Get the string for the given error
 * 
 * @param errorCodes code listet in ErrorCodes
 * @return const uint8_t* error message
 */
uint8_t* getErrorMessage(ErrorCodes errorCodes);
/**
 * @brief Function used to reset every field on th eboard to EMPTY_FILED
 * 
 * @param board that you want to erase
 */
void emptyBoard(uint8_t board[][SIZE]);
/**
 * @brief Function that is checking messages received from the PC
 * 
 * @param message message received from the board
 * @param messageFromPCType one of the possible messages that the user can type in
 * @return true 
 * @return false 
 */
bool checkMessageTypeFromPC(const uint8_t message[], MessageFromPCType messageFromPCType);
/**
 * @brief Function that is checking messages received from the enemy board
 * 
 * @param message received from the other board
 * @param messageFromBoardType one of the possible messages that the user can type in
 * @return true
 * @return false 
 */
bool checkMessageTypeFromBoard(const uint8_t message[], MessageFromBoardType messageFromBoardType);
/**
 * @brief Function used to clear a message/continer of chars
 * 
 * @param cont container to be cleared
 * @param size of the container
 */
void emptyContainer(uint8_t cont[], uint8_t size);
/**
 * @brief Used to copy a character stream and ignore the '\0' characters
 * 
 * @param dest to be copied
 * @param res copy from
 * @param size size of the res container
 */
void copyContainer(uint8_t dest[], const uint8_t res[], uint8_t size);
/**
 * @brief Function used to check if two containers are equal
 * 
 * @param buff1 
 * @param buff2 
 * @param size size/length of the containers it can either be the full size or justsome elements
 * @return true 
 * @return false 
 */
bool equalContainer(const uint8_t buff1[], const uint8_t buff2[], const uint8_t size);
/**
 * @briefFunction that writes W:Req into msg
 * 
 * @param msg 
 */
void writeWinReqMessage(uint8_t msg[]);
/**
 * @brief Function used to make error message that is going to be send to the host PC
 * 
 * @param errorMsg place where the error message is going to be stored
 * @param error type of error that has occured
 */
void makeErrorMessagePC(uint8_t errorMsg[], ErrorCodes error);
/**
 * @brief Function used to write an error message that is going to be send to the other board
 * 
 * @param message char that is describing the type of message where the errror has occured
 * @param errorMsg place to store the error message
 * @param error error which has occured on the recieved message
 */
void makeErrorMessage(uint8_t message, uint8_t errorMsg[], ErrorCodes error);
/**
 * @brief Function used to write an ok message that is going to be send to the other board
 * 
 * @param message type of message that is okey
 * @param okMsg place to store the error message
 */
void makeOkeyMessage(uint8_t message, uint8_t okMsg[]);
/**
 * @brief Function that takes an echo message received from the other board
 * and copies it into another stream, stream used to send data to te host pc
 * 
 * @note in addition this function erases the crc code of the recieved message
 * 
 * @param message echo message recieved from the board
 * @param messageToBeCreated place store the board
 * @return ErrorCodes 
 */
ErrorCodes makeEchoMessage(uint8_t message[], uint8_t messageToBeCreated[]);
/**
 * @brief Function to check the type of ok message
 * 
 * @note used to find out what was the message that was send last time
 * 
 * @param message ok message recieved from the othe board
 * @return ErrorCodes 
 */
ErrorCodes okMessageType(uint8_t message[]);
/**
 * @brief Function used to write a victory message that is going to inform the player that he has won the game
 * 
 * @param message 
 */
void makeVictoryMessage(uint8_t message[]);
/**
 * @brief Function used to writen a victory confirmation message form "V:Confirmation"
 * 
 * @param message place to store the victor confirmation message
 */
void makeVictoryConfirmationMessage(uint8_t message[]);
/**
 * @brief Get the Num Of Ships object
 * 
 * @param board 
 * @return uint8_t represents number of BOAT_FIELD on the given board
 */
uint8_t getNumOfShips(uint8_t board[][SIZE]);
/**
 * @brief Function used to make each field on the board to a EMPTY_FIELD
 * 
 * @return uint8_t 
 */
void clearAllShips(void);
/**
 * @brief Get the Num Of Placed Ships object
 * 
 * @return uint8_t represents number of ships placed on the board
 */
uint8_t getNumOfPlacedShips(void);
/**
 * @brief Functionn used to check if two board match
 * 
 * @param b1 
 * @param b2 
 * @return true 
 * @return false 
 */
bool equalBoards(uint8_t b1[][SIZE], uint8_t b2[][SIZE]);
/**
 * @brief function used to check if a container is empty
 * 
 * @note by empty it means if it is full with '\0'
 * 
 * @param cont continer whose emptiness we are checking
 * @param size size of cont
 * @return true 
 * @return false 
 */
bool isEmpty(uint8_t cont[], uint8_t size);
/**
 * @brief Function used to find a random number that is differenet from the one given via
 * 
 * @param num 
 * @return uint8_t 
 */
uint8_t findRandomNumber(uint8_t num);

#endif


