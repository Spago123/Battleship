#include "test_parser.h"

typedef bool (*Test)(void);

///@note tests 1 to 8 are testing board setup function

/// @brief placing ok code
static bool test1(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE];
	emptyBoard(board);
	if(placeShip2((uint8_t*)"BSR:C;A;0;0", board) == ERROR_NONE) return true;
	return false;
}

/// @brief placing wrong
static bool test2(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE];
	emptyBoard(board);
	if(placeShip2((uint8_t*)"BSR:C;A;0;1", board) != ERROR_NONE) return true;
	return false;
}

/// @brief testing clearAllShipsFunction, should be two if everything is clear, and then zero
static bool test3(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE];
	placeShip2((uint8_t*)"BSR:D;A;0;3", board);
	placeShip2((uint8_t*)"BSR:P;D;5;3", board);
	if(getNumOfPlacedShips() != 2) return false;
	clearAllShips();
	return getNumOfPlacedShips() == 0;
}


static bool test4(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE];
	placeShip2((uint8_t*)"BSR:B;A;0;3", board);
	placeShip2((uint8_t*)"BSR:B;E;0;3", board);
	
	return getNumOfPlacedShips() == 1;
}

/// @brief pacing a ship, the replacing that ship wrong, then correct 
static bool test5(void){
	clearAllShips();
	uint8_t expected[SIZE][SIZE], gotten[SIZE][SIZE];
	emptyBoard(expected);emptyBoard(gotten);
	placeShip2((uint8_t*)"BSR:P;A;0;0", gotten);
	expected[0][0] = BOAT_FIELD; expected[0][1] = BOAT_FIELD;
	if(!equalBoards(expected, gotten)) return false;
	
	placeShip2((uint8_t*)"BSR:P;D;0;1", gotten);
	if(!equalBoards(expected, gotten)) return false;
	
	placeShip2((uint8_t*)"BSR:P;D;0;3", gotten);
	if(equalBoards(expected, gotten)) return false;
	
	emptyBoard(expected);
	expected[0][3] = BOAT_FIELD;expected[1][3] = BOAT_FIELD;
	
	return equalBoards(expected, gotten);
}

static bool test6(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE], expected[SIZE][SIZE];
	expected[0][0] = BOAT_FIELD; expected[0][1] = BOAT_FIELD; expected[0][2] = BOAT_FIELD;expected[0][3] = BOAT_FIELD;expected[0][4] = BOAT_FIELD;
	expected[1][0] = BOAT_FIELD; expected[1][1] = BOAT_FIELD; expected[1][2] = BOAT_FIELD;expected[1][3] = BOAT_FIELD;
	expected[2][0] = BOAT_FIELD; expected[2][1] = BOAT_FIELD; expected[2][2] = BOAT_FIELD;
	expected[3][0] = BOAT_FIELD; expected[3][1] = BOAT_FIELD; expected[3][2] = BOAT_FIELD;
	expected[4][0] = BOAT_FIELD; expected[4][1] = BOAT_FIELD; 
	emptyBoard(board);
	placeShip2((uint8_t*)"BSR:C;A;0;0", board);
	if(placeShip2((uint8_t*)"BSR:B;A;0;0", board) == ERROR_NONE) return false;
	if(placeShip2((uint8_t*)"BSR:B;E;0;0", board) == ERROR_NONE) return false;
	placeShip2((uint8_t*)"BSR:B;A;1;0", board);
	placeShip2((uint8_t*)"BSR:D;A;2;0", board);
	placeShip2((uint8_t*)"BSR:S;A;3;0", board);
	placeShip2((uint8_t*)"BSR:P;A;4;0", board);
	return getNumOfPlacedShips() == 5 && equalBoards(board, expected);
}

static bool test7(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE], expected[SIZE][SIZE];
	expected[0][0] = BOAT_FIELD; expected[0][1] = BOAT_FIELD; expected[0][2] = BOAT_FIELD;expected[0][3] = BOAT_FIELD;expected[0][4] = BOAT_FIELD;
	expected[1][0] = BOAT_FIELD; expected[1][1] = BOAT_FIELD; expected[1][2] = BOAT_FIELD;expected[1][3] = BOAT_FIELD;
	expected[2][0] = BOAT_FIELD; expected[2][1] = BOAT_FIELD; expected[2][2] = BOAT_FIELD;
	expected[3][0] = BOAT_FIELD; expected[3][1] = BOAT_FIELD; expected[3][2] = BOAT_FIELD;
	expected[4][0] = BOAT_FIELD; expected[4][1] = BOAT_FIELD; 
	emptyBoard(board);
	placeShip2((uint8_t*)"BSR:C;A;0;0", board);
	placeShip2((uint8_t*)"BSR:B;A;1;0", board);
	placeShip2((uint8_t*)"BSR:D;A;2;0", board);
	placeShip2((uint8_t*)"BSR:S;A;3;0", board);
	placeShip2((uint8_t*)"BSR:P;A;4;0", board);
	
	
	placeShip2((uint8_t*)"BSR:C;J;0;0", board);
	placeShip2((uint8_t*)"BSR:B;J;1;0", board);
	placeShip2((uint8_t*)"BSR:D;J;2;0", board);
	placeShip2((uint8_t*)"BSR:S;J;3;0", board);
	placeShip2((uint8_t*)"BSR:P;J;4;0", board);
	
	return getNumOfPlacedShips() == 5 && equalBoards(board, expected);
	
}

static bool test8(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE];
	return placeShip2((uint8_t*)"BSR:B;A;-1;0", board) == INVALID_MSG;
}

/// @brief testing player guess from the input
///        testing if hit occured, if turn has changed, and if the appropriate message has been created 
static bool test9(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE], expected[SIZE][SIZE];
	emptyBoard(board); emptyBoard(expected);
	placeShip2((uint8_t*)"BSR:P;A;0;0", board);
	expected[0][0] = BOAT_FIELD; expected[0][1] = SINK_FIELD;
	uint8_t expectedCreatedMsg[MAX_MSG] = "G:B;0", messageToBeCreated[MAX_MSG];
	appendCrcCode(expectedCreatedMsg);
	uint8_t placedShips = getNumOfShips(board);
	bool myTurn = true;
	
	handlePlayerGuess((uint8_t*)"PGS:B;0", messageToBeCreated, board, &placedShips, &myTurn);
	
	if(!equalBoards(board, expected)) return false;
	
	if(!equalContainer(expectedCreatedMsg, messageToBeCreated, MAX_MSG)) return false;
	
	
	return placedShips == 1 && !myTurn;
}

/// @brief more advanced test testingif corrcetliy hit/miss the field or  
static bool test10(void){
	clearAllShips();
	uint8_t board[SIZE][SIZE], expected[SIZE][SIZE];
	emptyBoard(board); emptyBoard(expected);
	placeShip2((uint8_t*)"BSR:P;A;0;0", board);
	uint8_t placedShips = getNumOfShips(board);
	bool myTurn = true;
	
	uint8_t expectedCreatedMsg[MAX_MSG] = "G:B;1", messageToBeCreated[MAX_MSG];
	
	handlePlayerGuess((uint8_t*)"PGS:B;1", messageToBeCreated, board, &placedShips, &myTurn);
	
	if(board[1][1] != MISS_FIELD) return false;
	
	handlePlayerGuess(messageToBeCreated, messageToBeCreated, expected, &placedShips, &myTurn);
	
	if(expected[1][1] != MISS_FIELD)return false;
	
	handlePlayerGuess((uint8_t*)"PGS:B;0", messageToBeCreated, board, &placedShips, &myTurn);	
	
	uint8_t priv = placedShips;
	
	if(myTurn) return false;
	
	myTurn = true;
	
	handlePlayerGuess((uint8_t*)"PGS:B;0", messageToBeCreated, board, &placedShips, &myTurn);	
	
	return priv == placedShips && board[0][1] == SINK_FIELD;
}

/// @brief test that is checking whether the error message is correctly made for the board, and the pc
static bool test11(void){
	uint8_t errorMessage[MAX_MSG];
	//SIMULATING WRONG MESSAGE
	uint8_t isReceived[MAX_MSG] = "G:A;0";
	if(checkCrc(isReceived)) return false;
	makeErrorMessage(isReceived[0], errorMessage, CRC_CHECK_FAILED);
	uint8_t expectedError[MAX_MSG] = "P:G:102";
	appendCrcCode(expectedError);
	if(!equalContainer(errorMessage, expectedError, MAX_MSG)) return false;
	
	uint8_t pcError[MAX_MSG] = "";
	handleErrorFromBoard(errorMessage, pcError);
	uint8_t expectedErrorPC[MAX_MSG] = "ERR:102";
	
	return equalContainer(pcError, expectedErrorPC, MAX_MSG);
}

/// @brief test that is checking if error message for not equal boards is correctly made
static bool test12(void){
	uint8_t expectedMsg[MAX_MSG] = "P:A:101";
	appendCrcCode(expectedMsg);
	uint8_t errorMade[MAX_MSG];
	makeErrorMessage(BOARD_A, errorMade, BOARDS_DO_NOT_MATCH);
	
	if(!equalContainer(expectedMsg, errorMade, MAX_MSG)) return false;
	
	uint8_t expectedMsgPC[MAX_MSG] = "ERR:101";
	uint8_t errorMadePC[MAX_MSG];
	handleErrorFromBoard(errorMade, errorMadePC);
	return equalContainer(errorMadePC, expectedMsgPC, MAX_MSG);
}

static bool test13(void){
		uint8_t expectedMsg[MAX_MSG] = "P:A:105";
	appendCrcCode(expectedMsg);
	uint8_t errorMade[MAX_MSG];
	makeErrorMessage(BOARD_A, errorMade, INVALID_MSG);
	
	if(!equalContainer(expectedMsg, errorMade, MAX_MSG)) return false;
	
	uint8_t expectedMsgPC[MAX_MSG] = "ERR:105";
	uint8_t errorMadePC[MAX_MSG];
	handleErrorFromBoard(errorMade, errorMadePC);
	return equalContainer(errorMadePC, expectedMsgPC, MAX_MSG);
}

Test allTests[NUM_OF_TESTS] = {test1, test2, test3, test4, test5, test6, test7, test8, test9, test10, test11, test12, test13};

bool runTests(void){
	for(uint8_t i = 0; i < NUM_OF_TESTS; i++)
		if(!allTests[i]()) return false;
	return true;
}