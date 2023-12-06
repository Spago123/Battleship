#include "battleship.h"
#include "parser.h"
#include "connector.h"

/**
 * @brief Functionused to send both boards to the PC
 * 
 * @return ErrorCodes 
 */
static ErrorCodes sendBoardsToPC(void);
/**
 * @brief Get the Board Name object function used to decide who is going to be called BOARD_A and who BOARD_B
 * 
 * @note bigger number is going to be BOARD_A
 * 
 * @param myNumber 
 * @param enemyNumber 
 * @return uint8_t 
 */
static uint8_t getBoardName(uint8_t myNumber, uint8_t enemyNumber);
/**
 * @brief Function used to check whether the board is equal to the board received that has been 
 * received from the other board through the recievedMsgFromBoard
 * 
 * @param board 
 * @return true 
 * @return false 
 */
static bool checkBoards(uint8_t board);
/**
 * @brief Function used send a board whose name is given via the letter to the PC
 * 
 * @param letter 
 */
static void sendBoardToPC(uint8_t letter);
/**
 * @brief Fubction used to clear the boards, to set the number of ships to zero, clear all char buffers etc.
 * 
 */
static void resett(void);

ErrorCodes error;
Connector* board2board;
Connector* board2PC;
bool myTurn;

static bool foundGame = false;
static bool pingging = false;

/// @brief Variables used to store data transmited between boards, and flags
uint8_t receivedMsgFromBoard[MAX_MSG], sendMsgToBoard[MAX_MSG];
bool receivedFromBoard = false, sendFromBoard = false;
/// 

/// @brief Variable used to store data transmited between board and PC, and flags
uint8_t receivedMsgFromPC[MAX_MSG], sendMsgToPC[MAX_MSG];
bool receivedFromPC = false, sendToPC = false;
///

static uint8_t PINGGING_CODE[MAX_MSG] = "PLAY";
static uint8_t msg[MAX_MSG] = "Place your ships!";
static uint8_t goFirst[MAX_MSG] = "You are first";
static uint8_t goSecond[MAX_MSG] = "You are second";
static bool doit = true;

uint8_t okeyCode[MAX_MSG];
bool okeyCodeOccured = false;

uint8_t sendRandom = 0, receiveRandom = 0, temp = 0;

uint8_t numOfShips = NUM_SHIPS;
uint8_t availableShips[NUM_SHIPS];

GamePhase gamePhase = STEADY;
GameSubPhase subPhase = UNDEFINED;
TurnPhase boardCommState = USER_INPUT;
TurnPhase pcState = USER_INPUT;
PrivState privState = WORKER_LINE;


uint8_t myBoardLeftBoats; /// Number of boats left on my board
uint8_t enemyBoardLeftBoats; /// Number of boats left on enemies board

uint8_t myBoardName;
uint8_t enemyBoardName;

uint8_t myBoard[SIZE][SIZE];
uint8_t enemyBoard[SIZE][SIZE];

void playTheGame()
{
	board2board = setUp(CON_USART2, FAST);
	board2PC = setUp(CON_USART1, FAST);
	
	emptyBoard(myBoard);
	emptyBoard(enemyBoard);	
	
	appendCrcCode(PINGGING_CODE);
	initButton(GPIOA, B1_Pin);
	/// INIT RANODM NUM GENERATOR
	srand(0);
	while (1)
	{
		switch (gamePhase)
		{
		case STEADY:
			HAL_GPIO_WritePin(gameFound_GPIO_Port, gameFound_Pin, GPIO_PIN_RESET);
			break;
		case PINGINIG_FOR_GAME:
			
			if(privState == BOSS_LINE && doit){
				doit = false;
				receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
			}else if(privState == WORKER_LINE && doit){
				doit = false;
				transmitData(board2board, PINGGING_CODE, MAX_MSG);
				makeOkeyMessage(PINGGING_CODE[0], okeyCode);
				receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
			}
			
			if(receivedFromBoard && privState == BOSS_LINE){
				receivedFromBoard = false;
				if(equalContainer(PINGGING_CODE, receivedMsgFromBoard, MAX_MSG)){
					makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
					receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
					makeOkeyMessage(sendMsgToBoard[0], okeyCode);
					transmitData(board2board, sendMsgToBoard, MAX_MSG);
				}else if(equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
					gamePhase = FOUND_GAME;
					doit = true;
				}else doit = true;
			}else if(receivedFromBoard && privState == WORKER_LINE){
				receivedFromBoard = false;
				if(equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
					makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
					transmitData(board2board, sendMsgToBoard, MAX_MSG);
					gamePhase = FOUND_GAME;
					doit = true;
				}else doit = true;
			}
			break;
		case FOUND_GAME:
			HAL_GPIO_WritePin(gameFound_GPIO_Port, gameFound_Pin, GPIO_PIN_SET);
		
			if(privState == WORKER_LINE && doit){
				doit = false;
				receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
			}else if(privState == BOSS_LINE && doit){
				doit = false;
				emptyContainer(sendMsgToBoard, MAX_MSG);
				sendMsgToBoard[0] = rand()%255;
				receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
				appendCrcCode(sendMsgToBoard);
				makeOkeyMessage(sendMsgToBoard[0], okeyCode);
				transmitData(board2board, sendMsgToBoard, MAX_MSG);
			}
			
			if(receivedFromBoard){
				receivedFromBoard = false;
				if(privState == WORKER_LINE){
					if(checkCrc(receivedMsgFromBoard) && !equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
						uint8_t random = findRandomNumber(receivedMsgFromBoard[0]);
						myBoardName = getBoardName(random, receivedMsgFromBoard[0]);
						enemyBoardName = getBoardName(receivedMsgFromBoard[0], random);
						makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
						transmitData(board2board, sendMsgToBoard, MAX_MSG);
						receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
						HAL_Delay(10);
						emptyContainer(sendMsgToBoard, MAX_MSG);
						sendMsgToBoard[0] = random;
						appendCrcCode(sendMsgToBoard);
						makeOkeyMessage(random, okeyCode);
						transmitData(board2board, sendMsgToBoard, MAX_MSG);
					}else if(checkCrc(receivedMsgFromBoard) && equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
						gamePhase = TRANSIT_STATE;
					}
				}else if(privState == BOSS_LINE){
					if(equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
						receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
					}else if(checkCrc(receivedMsgFromBoard) && !equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
						myBoardName = getBoardName(okeyCode[2], receivedMsgFromBoard[0]);
						enemyBoardName = getBoardName(receivedMsgFromBoard[0], okeyCode[2]); 
						makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
						transmitData(board2board, sendMsgToBoard, MAX_MSG);
						HAL_Delay(10);
						gamePhase = TRANSIT_STATE;
					}
				}
			}
			break;
			
		case TRANSIT_STATE:
			transmitData(board2PC, msg, MAX_MSG);
			subPhase = (myBoardName == BOARD_A) ? PLAYING_MODE_A : WAITING_MODE_B;
			myTurn = (myBoardName == BOARD_A);
			emptyContainer(receivedMsgFromBoard, MAX_MSG);
			emptyContainer(okeyCode, MAX_MSG);
			emptyContainer(sendMsgToBoard, MAX_MSG);
			receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
			receiveData(board2PC, receivedMsgFromPC, MAX_MSG);
			HAL_GPIO_WritePin(boss_GPIO_Port, boss_Pin, GPIO_PIN_RESET);
			gamePhase = SETTING_UP_THE_BOARD;
			break;
		case SETTING_UP_THE_BOARD:
			if(receivedFromPC){
				receivedFromPC = false;
				error = parseUserInput(receivedMsgFromPC);
				if(error == RESET_BOARD || error == RESET_GAME){
					error = ERROR_NONE;
					emptyBoard(myBoard);
				}
				if(error == READY_MSG){
					makeErrorMessagePC(sendMsgToPC, ERROR_NONE); 
				}else{
					makeErrorMessagePC(sendMsgToPC, error);     
				}					
				transmitData(board2PC, sendMsgToPC, MAX_MSG);
				
			if(numOfShips == 0 || error == READY_MSG) {
				numOfShips = NUM_SHIPS;
				myBoardLeftBoats = getNumOfShips(myBoard);
				HAL_Delay(8);
				sendBoardToPC('M');
				HAL_Delay(8);
				if(myTurn){
					transmitData(board2PC, goFirst, MAX_MSG);
				}else if(!myTurn){
					transmitData(board2PC, goSecond, MAX_MSG);
				}
				gamePhase = EXCHANGE_BOARDS;
				pcState = USER_INPUT;
				boardCommState = USER_INPUT;
				break;
			}
			receiveData(board2PC, receivedMsgFromPC, MAX_MSG);
		}

			break;
		case EXCHANGE_BOARDS:
			switch (subPhase){
				case UNDEFINED:
					gamePhase = STEADY;
					break;
				case PLAYING_MODE_A:
					receivedFromBoard = false;
					packBoardMsg(sendMsgToBoard, myBoardName, myBoard, false);
					transmitData(board2board, sendMsgToBoard, MAX_MSG);
					receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
					makeOkeyMessage(myBoardName, okeyCode);
					subPhase = INCOMING;
					break;
				case WAITING_MODE_B:
					if(receivedFromBoard){
						subPhase = CRC_CHECK;
						break;
					}
					receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
					subPhase = CRC_CHECK;
					break;
				case INCOMING:

					if(receivedFromBoard){
						receivedFromBoard = false;
						if(equalContainer(okeyCode, receivedMsgFromBoard, MAX_MSG)){
							gamePhase = (myBoardName == BOARD_A) ? EXCHANGE_BOARDS : PLAYING_GAME; 
							subPhase = WAITING_MODE_B;
						}else{
							subPhase = PLAYING_MODE_A;
						}
					}
				case CRC_CHECK:
					if(receivedFromBoard){
						receivedFromBoard = false;
						if(checkCrc(receivedMsgFromBoard)){
							subPhase = PLAYING_MODE_A;
							makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
							transmitData(board2board, sendMsgToBoard, MAX_MSG);
							HAL_Delay(8);
							decompressBoard(enemyBoard, receivedMsgFromBoard + 2);
							enemyBoardLeftBoats = getNumOfShips(enemyBoard);
							gamePhase = (myBoardName == BOARD_A) ? PLAYING_GAME : EXCHANGE_BOARDS; 
						}else{
							subPhase = WAITING_MODE_B;
						}
					}
			}
			break;
				case PLAYING_GAME:
					stateMachinePC();
					stateMachineBoardComm();
			break;
		}
	}
}

void USART1_TxCallback()
{
	sendToPC = true;
}

void USART1_RxCallback(){
	receivedFromPC = true;
}

/// @brief Board to board conversation
void USART2_RxCallback()
{
	receivedFromBoard = true;
}

void USART2_TxCallback()
{
	sendFromBoard = true;
}

/// @brief function that is called when the button is pressed and then the board starts searching for a game
void buttonPressed()
{
	if(HAL_GPIO_ReadPin(worker_GPIO_Port, worker_Pin) == GPIO_PIN_RESET){
		privState = BOSS_LINE;
		HAL_GPIO_WritePin(boss_GPIO_Port, boss_Pin, GPIO_PIN_SET);
	}else{
		privState = WORKER_LINE;
	}

	gamePhase = (gamePhase != STEADY) ? STEADY : PINGINIG_FOR_GAME;
	emptyContainer(receivedMsgFromBoard, MAX_MSG);
	receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
}

static uint8_t getBoardName(uint8_t myNumber, uint8_t enemyNumber){
	if(myNumber > enemyNumber) return BOARD_A;
	else if(myNumber < enemyNumber) return BOARD_B;
	return 0;
}


void packBoardMsg(uint8_t message[MAX_MSG], uint8_t boardName, uint8_t board[][SIZE], bool hideShips){
	message[0] = boardName; message[1] = ':';
	compressBoard(board, message + 2, hideShips);
	appendCrcCode(message);
}


void sendBoardToPC(uint8_t letter){
    sendMsgToPC[0] = letter;
    sendMsgToPC[1] = ':';
	if(letter == 'M'){
    	compressBoard(myBoard, sendMsgToPC + 2, false);
    	transmitData(board2PC, sendMsgToPC, MAX_MSG);
	}else if(letter == 'O'){
    	compressBoard(enemyBoard, sendMsgToPC + 2, true);
    	transmitData(board2PC, sendMsgToPC, MAX_MSG);
	}
}


bool checkBoards(uint8_t board){
	uint8_t compressedBoardData[25];
	if(board == myBoardName){
		compressBoard(myBoard, compressedBoardData, false);
		return equalContainer(receivedMsgFromBoard + 2, compressedBoardData, 25);
	}else if(board == enemyBoardName){
		compressBoard(enemyBoard, compressedBoardData, false);
		return equalContainer(receivedMsgFromBoard + 2, compressedBoardData, 25);
	}
}


bool stateMachinePC(void){
	switch (pcState){
		case USER_INPUT:
			receiveData(board2PC, receivedMsgFromPC, MAX_MSG);
			pcState = RECEIVING;
			break;
		case RECEIVING:
			if(receivedFromPC){
				receivedFromPC = false;
				pcState = PARSING;
			}
			break;
		case PARSING:
		  error  = parseUserInput(receivedMsgFromPC);
			if(!checkErrorCode(error) || error == RESET_BOARD){
				if(error == RESET_BOARD) error = INVALID_MSG;
				makeErrorMessagePC(sendMsgToPC, error);
				transmitData(board2PC, sendMsgToPC, MAX_MSG);
				pcState = USER_INPUT;
				break;
			}
			
			if(error == UPD_BOARD){
				pcState = USER_INPUT;
				break;
			}
			pcState = SENDING;
			break;
		case SENDING:
			makeOkeyMessage(sendMsgToBoard[0], okeyCode);
			HAL_Delay(20);
			transmitData(board2board, sendMsgToBoard, MAX_MSG);
			pcState = OK_CONFIRMATION;
			if(error == RESET_GAME_YES){
				resett();
			}
			break;
		case OK_CONFIRMATION:
			//// Basiclly do nothing till the other board says it's ok
			if(okeyCodeOccured){
				okeyCodeOccured = false;
				ErrorCodes okMsgType = okMessageType(receivedMsgFromBoard);
				emptyContainer(okeyCode, MAX_MSG);
				
				if(okMsgType == GUESS_DETECTED){
					/// TODO send message that contains A board data
					packBoardMsg(receivedMsgFromPC, 
					(myBoardName == BOARD_A) ? myBoardName : enemyBoardName,
					(myBoardName == BOARD_A) ? myBoard : enemyBoard, false);
					receivedFromPC = true;
					pcState = RECEIVING;
					break;
				}else if(okMsgType == A_MESSAGE){
					/// TODO send message that contains B board data
					packBoardMsg(receivedMsgFromPC, 
					(myBoardName == BOARD_B) ? myBoardName : enemyBoardName,
					(myBoardName == BOARD_B) ? myBoard : enemyBoard, false);
					receivedFromPC = true;
					pcState = RECEIVING;
					break;
				}else if(okMsgType == B_MESSAGE && enemyBoardLeftBoats == 0){
					/// TODO send out win request check message
					writeWinReqMessage(receivedMsgFromPC);
					receivedFromPC = true;
					pcState = RECEIVING;
					break;
				}else if(okMsgType == V_CONFIRMED){
					resett();
				}
				pcState = USER_INPUT;
			}
			break;
		default:
			break;
	}
}

bool stateMachineBoardComm(void){
	switch (boardCommState){
		case USER_INPUT:
			receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
			boardCommState = RECEIVING;
			break;
		case RECEIVING:
			if(receivedFromBoard){
				receivedFromBoard = false;
				if(!checkCrc(receivedMsgFromBoard)){
					makeErrorMessage(receivedMsgFromBoard[0], sendMsgToBoard, CRC_CHECK_FAILED);
					boardCommState = SENDING;
					break;
				}

				if(equalContainer(receivedMsgFromBoard, okeyCode, MAX_MSG)){
					okeyCodeOccured = true;
					boardCommState = USER_INPUT;
					break;
				}
				boardCommState = PARSING;
			}
			break;
		case SENDING:
			transmitData(board2board, sendMsgToBoard, MAX_MSG);
			boardCommState = USER_INPUT;
			if(okMessageType(sendMsgToBoard) == V_CONFIRMED){
				resett();
			}
			break;
		case PARSING:
			error = parseBoardInput(receivedMsgFromBoard);
			if(!checkErrorCode(error)){
				makeErrorMessage(receivedMsgFromBoard[0], sendMsgToBoard, error);
				boardCommState = SENDING;
				break;
			}
			
			if(error == RESET_GAME_YES){
				resett();
			}
			makeOkeyMessage(receivedMsgFromBoard[0], sendMsgToBoard);
			boardCommState = SENDING;
			break;
		default:
			break;
	}
}


ErrorCodes sendBoardsToPC(void){
	sendBoardToPC('M');
	HAL_Delay(8);
	emptyContainer(sendMsgToPC, MAX_MSG);
	sendBoardToPC('O');
	HAL_Delay(8);
	return UPD_BOARD;
}

ErrorCodes parseUserInput(uint8_t message[]){
		if(checkMessageTypeFromPC(message, BOARD_SET_UP)){
			return placeShip2(message, myBoard);
		}else if(checkMessageTypeFromPC(message, PLAYER_GUESS) && gamePhase == PLAYING_GAME){
			return handlePlayerGuess(message, sendMsgToBoard, enemyBoard, &enemyBoardLeftBoats, &myTurn);
		}else if(checkMessageTypeFromPC(message, RESET_REQUEST)){
			return handleReq(message, sendMsgToBoard);
		}else if(checkMessageTypeFromPC(message, UPDATE_BOARD)){
			return sendBoardsToPC();
		}else if(checkMessageTypeFromPC(message, ECHO_MSG)){
			return handleEchoMessage(message, sendMsgToBoard);
		}else if(checkMessageTypeFromBoard(message, BOARD_A_DATA) || 
						 checkMessageTypeFromBoard(message, BOARD_B_DATA) ||
						 checkMessageTypeFromBoard(message, WIN_CHECK) ||
						 checkMessageTypeFromBoard(message, VICTORY_CONFIRMED)){
						 HAL_Delay(10);
			copyContainer(sendMsgToBoard, message, MAX_MSG);
			return ERROR_NONE;
		}else if(checkMessageTypeFromPC(message, READY)){
			return READY_MSG;
		}
		
		return INVALID_MSG;
}


ErrorCodes parseBoardInput(uint8_t message[]){
	if(checkMessageTypeFromBoard(message, PLAYER_GUESS_BOARD) && gamePhase == PLAYING_GAME){
		ErrorCodes error = handlePlayerGuess(message, NULL, myBoard, &myBoardLeftBoats, &myTurn);
		if(error == ERROR_NONE){
			uint8_t yourTurn[MAX_MSG] = "RDT";
			if(!isEmpty(yourTurn + 3, MAX_MSG - 3)){
				emptyContainer(yourTurn + 3, MAX_MSG  - 3);
			}
			transmitData(board2PC, yourTurn, MAX_MSG);
			HAL_Delay(8);
		}
		return error;
	}else if(checkMessageTypeFromBoard(message, BOARD_A_DATA)){
		return checkIfBoardsMatch(message, (myBoardName == BOARD_A) ? myBoard : enemyBoard);
	}else if(checkMessageTypeFromBoard(message, BOARD_B_DATA)){
		return checkIfBoardsMatch(message, (myBoardName == BOARD_B) ? myBoard : enemyBoard);
	}else if(checkMessageTypeFromBoard(message, RESET_REQUEST_BOARD)){
		error = handleReqBoard(message, sendMsgToPC);
		transmitData(board2PC, sendMsgToPC, MAX_MSG);
		return error;
	}else if(checkMessageTypeFromBoard(message, ECHO_MSG_BOARD)){
		error = makeEchoMessage(message, sendMsgToPC);
		if(checkErrorCode(error)){
			transmitData(board2PC, sendMsgToPC, MAX_MSG);
		}
		return error;
	}else if(checkMessageTypeFromBoard(message, WIN_CHECK)){
			if(myBoardLeftBoats == 0){
				uint8_t youHaveLost[30] = "You have lost!";
				receivedFromPC = true;
				pcState = RECEIVING;
				makeVictoryConfirmationMessage(receivedMsgFromPC);
				transmitData(board2PC, youHaveLost, MAX_MSG);
				HAL_Delay(8);
				return ERROR_NONE;
			}
		return BOARDS_DO_NOT_MATCH;
	}else if(checkMessageTypeFromBoard(message, VICTORY_CONFIRMED)){
		makeVictoryMessage(sendMsgToPC);
		transmitData(board2PC, sendMsgToPC, MAX_MSG);
		return ERROR_NONE;
	}else if(checkMessageTypeFromBoard(message, OK_CODE)){
		/// THIS PART IS TESTED OUTSIDE OF THIS FUNCTION
	}else if(checkMessageTypeFromBoard(message, ERROR_CODE)){
		ErrorCodes error = handleErrorFromBoard(message, sendMsgToPC);
		if(!checkErrorCode(error)) return error;
		HAL_Delay(8);
		transmitData(board2PC, sendMsgToPC, MAX_MSG);
		return error;
	}
	
	return INVALID_MSG;
}

void resett(void){
	emptyBoard(myBoard);
	emptyBoard(enemyBoard);
	numOfShips = NUM_SHIPS;
	gamePhase = SETTING_UP_THE_BOARD;
	error = ERROR_NONE;
	myBoardLeftBoats = 0;
	enemyBoardLeftBoats = 0;
	receivedFromPC = false;
	clearAllShips();
	receiveData(board2PC, receivedMsgFromPC, MAX_MSG);
	receiveData(board2board, receivedMsgFromBoard, MAX_MSG);
}





