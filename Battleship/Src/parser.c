#include "parser.h"

static ShipObject ships[5];
static uint8_t numOfplacedShips = 0;

static const uint8_t ERROR_MESSAGES[8][4] = {"100", "101", "102", "103", "104", "105", "106"};

/// @brief Strings used to parse input data from the PC
static const uint8_t MESSAGE_FROM_PC_TYPES[][30] = {"BSR:%c;%c;%c;%c", 
                                                    "PGS:%c;%c", 
                                                    "RST:%c",
																										"UPD",
																										"ECH:%s",
																										"W:Req",
																										"RDY"
                                                    };

static const uint8_t boardSetUpParams = 4;
static const uint8_t playerGuessParams = 2;
static const uint8_t resetReqParams = 1;
static const uint8_t echoParams = 1;


/// @brief Strings used to parse input from board
static const uint8_t MESSAGE_FROM_BOARD_TYPES[][30] = {"G:%c;%c", 
													   "A:%s", 
													   "B:%s",
													   "R:%c",
	                                                    "E:%s",
			                                           "W:%c", 
	                                                     "V:%c", 
	                                                     "K:%c", 
	"P:%c:%s"
                                                       };
static const uint8_t numOfParamsForEachMsg = 1;


static uint8_t getShipSize(uint8_t ship){
    if(ship == CARRIER) return 5;
    else if(ship == BATTLESHIP) return 4;
    else if(ship == DESTROYER) return 3;
    else if(ship == SUBMARINE) return 3;
    else if(ship == PATROL_BOAT) return 2;
    return 0;
}

static bool checkRotation(uint8_t rotation){
    return (rotation == ROTATION_0) || (rotation == ROTATION_90)
    || (rotation == ROTATION_180) || (rotation == ROTATION_270);
}

static void eraseShip(uint8_t ship, uint8_t availableShips[]){
    uint8_t i;
    for(i = 0; i < NUM_SHIPS; i++){
        if(ship == availableShips[i]) {
            availableShips[i] = NON;
            break;
        }
    }
}

static bool findShip(uint8_t ship, uint8_t availableShips[]){
    uint8_t i;
    for(i = 0; i < NUM_SHIPS; i++) {
        if(ship == availableShips[i])return true;
    }
    return false;
}
static int8_t checkBoard4Ship(uint8_t shipType, uint8_t board[][SIZE]){
	for(uint8_t i = 0; i < numOfplacedShips; i++){
		if(ships[i].T == shipType) return i;
	}
	return -1;
}

static void removeShip4Board(ShipObject ship, uint8_t board[][SIZE]){
	for(uint8_t i = ship.Px; i < ship.Px + getShipSize(ship.T) && ship.R == ROTATION_0; i++){
		board[ship.Py][i] = EMPTY_FIELD;
	}
	for(uint8_t i = ship.Py; i < ship.Py + getShipSize(ship.T) && ship.R == ROTATION_270; i++){
		board[i][ship.Px] = EMPTY_FIELD;
	}
}

static ErrorCodes placeShipObject(ShipObject ship, uint8_t board[][SIZE]){
	for(uint8_t i = ship.Px; i < ship.Px + getShipSize(ship.T) && ship.R == ROTATION_0; i++){
		if(board[ship.Py][i] == EMPTY_FIELD){
			board[ship.Py][i] = BOAT_FIELD;
		}else{
			int8_t j = i - 1;
			for(; j >= ship.Px; j--){
				board[ship.Py][j] = EMPTY_FIELD;
			}
			return INVALID_INPUT_VIA_TERMINAL;
		}
	}	
	
	for(uint8_t i = ship.Py; i < ship.Py + getShipSize(ship.T) && ship.R == ROTATION_270; i++){
		if(board[i][ship.Px] == EMPTY_FIELD){
			board[i][ship.Px] = BOAT_FIELD;
		}else{
			int8_t j = i - 1;
			for(; j >= ship.Py; j--){
				board[j][ship.Px] = EMPTY_FIELD;
			}
			return INVALID_INPUT_VIA_TERMINAL;
		}
	}
	return ERROR_NONE;
}

ErrorCodes placeShip2(uint8_t stringInput[], uint8_t board[][SIZE]){
		static uint8_t size = 0;
	  ShipObject shipObject;
    if(sscanf((const char*)stringInput, (const char*)MESSAGE_FROM_PC_TYPES[BOARD_SET_UP], &shipObject.T, &shipObject.Px, &shipObject.Py, &shipObject.R) != 4){
        return INVALID_MSG;
    }
		
		size = getShipSize(shipObject.T);
		if(size == 0) return INVALID_INPUT_VIA_TERMINAL;
    shipObject.Px = shipObject.Px - 'A';
    shipObject.Py = shipObject.Py - '0';
    shipObject.R = shipObject.R - '0';
		if(!checkRotation(shipObject.R)) return INVALID_INPUT_VIA_TERMINAL;
		
		if(shipObject.R == ROTATION_180) {
       if(shipObject.Px < (size - 1)) return INVALID_INPUT_VIA_TERMINAL;
       shipObject.Px -= (size - 1);
       shipObject.R = ROTATION_0;
    }
    if(shipObject.R == ROTATION_90){
        if(shipObject.Py < (size - 1)) return INVALID_INPUT_VIA_TERMINAL;
        shipObject.Py -= (size - 1);
        shipObject.R = ROTATION_270;
    }
		
    if(shipObject.R == ROTATION_0 && shipObject.Px + size > SIZE) return INVALID_INPUT_VIA_TERMINAL;
    if(shipObject.R == ROTATION_270 && shipObject.Py + size > SIZE) return INVALID_INPUT_VIA_TERMINAL;
		
		int8_t placedShipAdr = checkBoard4Ship(shipObject.T, board);
		if(placedShipAdr == -1){
			ErrorCodes error = placeShipObject(shipObject, board);
			if(error == ERROR_NONE)	ships[numOfplacedShips++] = shipObject;
			return error;
		}
		
		removeShip4Board(ships[placedShipAdr], board);
		ErrorCodes error = placeShipObject(shipObject, board);
		if(error != ERROR_NONE){
			placeShipObject(ships[placedShipAdr], board);
			return error;
		}
		
		ships[placedShipAdr] = shipObject;
		return error;
}


ErrorCodes handleReq(uint8_t message[], uint8_t messageToBeCreated[]){
    unsigned char req;
    if(sscanf((char*)message, (char*)MESSAGE_FROM_PC_TYPES[RESET_REQUEST], &req) != resetReqParams){
        return INVALID_MSG;
    }

    if(req == REQ_RESET_BOARD){
			messageToBeCreated = NULL;
      return RESET_BOARD;
    }else if(req == REQ_RESET_GAME){
        sprintf((char*)messageToBeCreated, (char*)MESSAGE_FROM_BOARD_TYPES[RESET_REQUEST_BOARD], req);
				appendCrcCode(messageToBeCreated);
        return RESET_GAME;
    }else if(req == REQ_RESET_GAME_YES){
			sprintf((char*)messageToBeCreated, (char*)MESSAGE_FROM_BOARD_TYPES[RESET_REQUEST_BOARD], req);
			appendCrcCode(messageToBeCreated);
			return RESET_GAME_YES;
		}else{
			sprintf((char*)messageToBeCreated, (char*)MESSAGE_FROM_BOARD_TYPES[RESET_REQUEST_BOARD], REQ_RESET_GAME_NO);
			appendCrcCode(messageToBeCreated);
			return RESET_GAME_NO;			
		}
    return INVALID_INPUT_VIA_TERMINAL;
}

ErrorCodes handleReqBoard(uint8_t message[], uint8_t messageToBeCreated[]){
	unsigned char req;
	if(sscanf((char*)message, (char*)MESSAGE_FROM_BOARD_TYPES[RESET_REQUEST_BOARD], &req) != resetReqParams){
		return INVALID_MSG;
  }
	
	if(req == REQ_RESET_GAME){
		uint8_t requestToPC[30] = "Can we start again?";
		copyContainer(messageToBeCreated, requestToPC, 30);
		return RESET_GAME;
	}else if(req == REQ_RESET_GAME_YES){
		uint8_t yes[30] = "Yes, let's start again.";
		copyContainer(messageToBeCreated, yes, 30);
		return RESET_GAME_YES;
	}else{
		uint8_t no[30] = "No, keep playing!";
		copyContainer(messageToBeCreated, no, 30);
		return RESET_GAME_NO;
	}
	return RESET_GAME_NO;
}

ErrorCodes handleErrorFromBoard(uint8_t message[], uint8_t messageToBeCreated[]){
	eraseCrcCode(message);
	const uint8_t msgTemplate[30] = "ERR:%s";
	uint8_t msgType, error[3];
	if(sscanf((char*)message, (char*)MESSAGE_FROM_BOARD_TYPES[ERROR_CODE], &msgType, error) != 2){
		return INVALID_MSG;
	}
	sprintf((char*)messageToBeCreated, (char*)msgTemplate, error);
	return ERROR_NONE;
}

ErrorCodes handlePlayerGuess(uint8_t message[], uint8_t messageToBeCreated[], uint8_t enemyBoard[][SIZE], uint8_t *leftHits, bool *myTurn){
    uint8_t Px, Py;
    if(*myTurn && sscanf((char*)message, (char*)MESSAGE_FROM_PC_TYPES[PLAYER_GUESS], &Px, &Py) == playerGuessParams){

    }else if(!*myTurn && sscanf((char*)message, (char*)MESSAGE_FROM_BOARD_TYPES[PLAYER_GUESS_BOARD], &Px, &Py) == playerGuessParams){
		
		} else{
			return INVALID_MSG;
		}

    Px -= 'A';
    Py -= '0'; 

    if(Px >= SIZE || Py >= SIZE){
        return PROPER_GUESS_INVALID_COORDINATES;
    }

    if(enemyBoard[Py][Px] == BOAT_FIELD){
        enemyBoard[Py][Px] = SINK_FIELD;
				(*leftHits)--;///DECREASE THE NUMBER OF HITS LEFT
    }else if(enemyBoard[Py][Px] == EMPTY_FIELD){
				enemyBoard[Py][Px] = MISS_FIELD;
    }

		if(*myTurn){
			emptyContainer(messageToBeCreated, 30);
			sprintf((char*)messageToBeCreated, (char*)MESSAGE_FROM_BOARD_TYPES[PLAYER_GUESS_BOARD], Px + 'A', Py + '0');
			appendCrcCode(messageToBeCreated);
		}
		*myTurn = !(*myTurn);
    return ERROR_NONE;
}

ErrorCodes handleEchoMessage(uint8_t message[], uint8_t messageToBeCreated[]){
	const uint8_t msgTemplate[] = "E:";
	copyContainer(messageToBeCreated, msgTemplate, 2);
	copyContainer(messageToBeCreated + 2, message + 4, 26);
	appendCrcCode(messageToBeCreated);
	return ERROR_NONE;
}

ErrorCodes checkIfBoardsMatch(uint8_t message[], uint8_t board[][SIZE]){
	uint8_t tmp[25];
	compressBoard(board, tmp, false);	
	return equalContainer(message + 2, tmp, 25) ? ERROR_NONE : BOARDS_DO_NOT_MATCH;
}


uint8_t* getErrorMessage(ErrorCodes errorCodes){
	return (uint8_t*)ERROR_MESSAGES[errorCodes];
}


void emptyBoard(uint8_t board[][SIZE]){
	uint8_t i, j;
	for(i = 0; i < SIZE; i++)
		for(j = 0; j < SIZE; j++)
			board[i][j] = ' ';
}

bool checkMessageTypeFromPC(const uint8_t message[], MessageFromPCType messageFromPCType){
    uint8_t* pointer1 = (uint8_t*)MESSAGE_FROM_PC_TYPES[messageFromPCType];
    uint8_t* pointer2 = (uint8_t*)message;
    for(;((*pointer1) != '\0' && (*pointer1)!= ':') && (*pointer1 == *pointer2); pointer1++, pointer2++);

	return ((*pointer1) == '\0') || ((*pointer1)== ':' && (*pointer1 == *pointer2));
}

bool checkMessageTypeFromBoard(const uint8_t message[], MessageFromBoardType messageFromBoardType){
    uint8_t* pointer1 = (uint8_t*)MESSAGE_FROM_BOARD_TYPES[messageFromBoardType];
    uint8_t* pointer2 = (uint8_t*)message;
		for(;((*pointer1) != '\0' && (*pointer1)!= ':') && (*pointer1 == *pointer2); pointer1++, pointer2++);

	return ((*pointer1) == '\0') || ((*pointer1)== ':' && (*pointer1 == *pointer2));
}

void emptyContainer(uint8_t cont[], uint8_t size){
	for(uint8_t i = 0; i < size; i++){
		cont[i] = '\0';
	}
}

void copyContainer(uint8_t dest[], const uint8_t res[], uint8_t size){
	for(uint8_t i = 0; i < size; i++){
		dest[i] = res[i];
	}
}

bool equalContainer(const uint8_t buff1[], const uint8_t buff2[], const uint8_t size){
    uint8_t* pointer1 = (uint8_t*)buff1;
    uint8_t* pointer2 = (uint8_t*)buff2;
    for(; pointer1 < buff1 + size && *pointer1 == *pointer2; pointer1++, pointer2++);
    return pointer1 == buff1 + size;
}


void writeWinReqMessage(uint8_t msg[]){
	const uint8_t victoryMsg[] = "W:Req";
	emptyContainer(msg, 30);
	copyContainer(msg, victoryMsg, 5);
	appendCrcCode(msg);
}

void makeErrorMessage(uint8_t message, uint8_t errorMsg[], ErrorCodes error){
		emptyContainer(errorMsg, 30);
    sprintf((char*)errorMsg, (char*)MESSAGE_FROM_BOARD_TYPES[ERROR_CODE], message, ERROR_MESSAGES[error]);
    appendCrcCode(errorMsg);
}


void makeErrorMessagePC(uint8_t errorMsg[], ErrorCodes error){
	emptyContainer(errorMsg, 30);
	if(error == READY_MSG || error == UPD_BOARD) error = ERROR_NONE;
	const uint8_t msgTemplate[30] = "ERR:%s";
	sprintf((char*)errorMsg, (char*)msgTemplate, ERROR_MESSAGES[error]);
}

void makeOkeyMessage(uint8_t message, uint8_t okMsg[]){
		emptyContainer(okMsg, 30);
    const uint8_t msgTemplate[30] = "K:%c";
    sprintf((char*)okMsg, (char*)msgTemplate, message);
    appendCrcCode(okMsg);
}

bool checkErrorCode(ErrorCodes error){
    return error == ERROR_NONE || error == RESET_BOARD 
            || error == RESET_GAME || error == UPD_BOARD
						|| error == GUESS_DETECTED || error == A_MESSAGE
						|| error == B_MESSAGE || error == W_MESSAGE
						|| error == V_CONFIRMED || error == READY_MSG
						|| error == RESET_GAME_YES || error == RESET_GAME_NO;
}

ErrorCodes makeEchoMessage(uint8_t message[], uint8_t messageToBeCreated[]){
	emptyContainer(messageToBeCreated, 30);
	copyContainer(messageToBeCreated, message, 30);
	eraseCrcCode(messageToBeCreated);
	return ERROR_NONE;
}

ErrorCodes okMessageType(uint8_t message[]){
	const uint8_t guess = 'G';
	const uint8_t boardA = 'A';
	const uint8_t boardB = 'B';
	const uint8_t vConfirmed = 'V';
	
	if(message[2] == guess) return GUESS_DETECTED;
	else if(message[2] == boardA) return A_MESSAGE;
	else if(message[2] == boardB) return B_MESSAGE;
	else if(message[2] == vConfirmed) return V_CONFIRMED;
	
	return ERROR_NONE;
}

void makeVictoryMessage(uint8_t message[]){
	const uint8_t msgTemplate[30] = "Congrats, you have won!!!";
	copyContainer(message, msgTemplate, 30);
}

void makeVictoryConfirmationMessage(uint8_t message[]){
	const uint8_t msgTemplate[30] = "V:Confirmation";
	copyContainer(message, msgTemplate, 30);
	appendCrcCode(message);
}

uint8_t getNumOfShips(uint8_t board[][SIZE]){
	uint8_t numOfShips = 0;
	for(uint8_t i = 0; i < SIZE; i++){
		for(uint8_t j = 0; j < SIZE; j++){
			if(board[i][j] == BOAT_FIELD){
				numOfShips++;
			}
		}
	}
	return numOfShips;
}
	

void clearAllShips(void){
	for(uint8_t i = 0; i < numOfplacedShips; i++)
		ships[i] = (ShipObject){NON, 0, 0, 0};
	numOfplacedShips = 0;
}

uint8_t getNumOfPlacedShips(void){
	return numOfplacedShips;
}

bool equalBoards(uint8_t b1[][SIZE], uint8_t b2[][SIZE]){
	for(uint8_t i = 0; i < SIZE; i ++)
		for(uint8_t j = 0; j < SIZE; j++)
			if(b1[i][j] != b2[i][j]) return false;
	
	return true;
}

bool isEmpty(uint8_t cont[], uint8_t size){
	for(uint8_t i = 0; i < 30; i++)
		if(cont[i] != '\0') return false;
	return true;
}

uint8_t findRandomNumber(uint8_t num){
	srand(0);
	uint8_t random;
	do{
		random = rand()%255;
	}while(random == num);
	
	return random;
}
																																				
																																				