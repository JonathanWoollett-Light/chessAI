#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct move {
	int startX;
	int startY;
	int endX;
	int endY;
	double value;
	move(int startX, int startY, int endX, int endY, double value) {
		this->startX = startX;
		this->startY = startY;
		this->endX = endX;
		this->endY = endY;
		this->value = value;
	}
};

double nextSetOfMoves(const int board[8][8], const int depth = 0);
move findMove(const int board[8][8], double routeValue);
double approximateBoard(const int board[8][8]);
double sigmoidFunction(double inp);
void makeMove();
void printBoard(const int board[8][8]);

std::vector<move> possibleMoves;

//odd is black, white is even
int startingBoard[8][8] = {
	{ 3,5,7,11,9,7,5,3 },
{ 1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 2,2,2,2,2,2,2,2 },
{ 4,6,8,10,12,8,6,4 },
};


bool playingBlack = false;

int peiceValues[5] = { 1,5,3,3,9 };

int maxDepth = 10;
int branchCounter = 0;
double modifier = 0.01;

int main() {

	nextSetOfMoves(startingBoard);
	makeMove();

	system("pause");
	return 0;
}

void makeMove() {
	int maxValueIndex = 0;
	std::cout << "\n\n" << std::endl;
	for (int i = 0; i < possibleMoves.size(); i++) {
		std::cout << "(" << possibleMoves[i].startX << ","
			<< possibleMoves[i].startY << ") -> ("
			<< possibleMoves[i].endX << ","
			<< possibleMoves[i].endY << ")\nvalue: " << possibleMoves[i].value << std::endl;
	}


	for (int i = 1; i < possibleMoves.size(); i++) {
		if (possibleMoves[i].value > possibleMoves[maxValueIndex].value) {
			maxValueIndex = i;
		}
	}

	printBoard(startingBoard);
	startingBoard[possibleMoves[maxValueIndex].endY][possibleMoves[maxValueIndex].endX] = startingBoard[possibleMoves[maxValueIndex].startY][possibleMoves[maxValueIndex].startX];
	startingBoard[possibleMoves[maxValueIndex].startY][possibleMoves[maxValueIndex].startX] = 0;
	printBoard(startingBoard);
	std::cout << "Best move:\n(" << possibleMoves[maxValueIndex].startX << ","
		<< possibleMoves[maxValueIndex].startY << ") -> ("
		<< possibleMoves[maxValueIndex].endX << ","
		<< possibleMoves[maxValueIndex].endY << ")\nvalue: " << possibleMoves[maxValueIndex].value << std::endl;
}

void printBoard(const int board[8][8]) {
	std::cout << "\n\n  0 1 2 3 4 5 6 7" << std::endl;
	std::cout << "  ________________" << std::endl;
	for (int y = 0; y < 8; y++) {
		std::cout << y << "|";
		for (int x = 0; x < 7; x++) {
			std::cout << board[y][x] << ",";
		}
		std::cout << board[y][7] << std::endl;
	}
}

move findMove(const int board[8][8], double routeValue) {
	int startX, startY, endX, endY;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (startingBoard[y][x] != board[y][x]) {
				if (board[y][x] == 0) {
					startX = x;
					startY = y;
				}
				else {
					endX = x;
					endY = y;
				}
			}
		}
	}
	return move(startX, startY, endX, endY, routeValue);
}

double approximateBoard(const int board[8][8]) {
	double value = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board[y][x] != 0 && board[y][x] != 11 && board[y][x] != 12) {
				if ((board[y][x] % 2 == 0 && playingBlack) || (board[y][x] % 2 == 1 && playingBlack)) {
					value += peiceValues[(int)ceil(board[y][x] / 2) - 1];
				}
				else {
					value -= peiceValues[(int)ceil(board[y][x] / 2) - 1];
				}
			}
		}
	}
	return sigmoidFunction(value);
}

double sigmoidFunction(double inp) {
	return (2 / (1 + pow(2.7182818284, -modifier * inp))) - 1;
}

double nextSetOfMoves(const int board[8][8], const int depth) {

	branchCounter++;
	if (branchCounter % 500000 == 0) {
		std::cout << "Depth: " << depth << std::endl;
		std::cout << "Branches: " << branchCounter << std::endl;
	}

	//checking if at max depth
	if (depth > maxDepth) {
		return approximateBoard(board);
	}
	//checking if kings alive
	bool blackKingsAlive = false;
	bool whiteKingAlive = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board[y][x] == 11) blackKingsAlive = true;
			if (board[y][x] == 12) whiteKingAlive = true;
		}
	}
	if (!blackKingsAlive) {
		if (playingBlack) return -1;
		return 1;
	}
	else if (!whiteKingAlive) {
		if (playingBlack) return 1;
		return -1;
	}
	//boardHolder to avoid reference issues
	int boardHolder[8][8];
	std::memcpy(boardHolder, board, 8 * 8 * sizeof int());
	int newBoard[8][8];
	std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
	double routeValue = 0;
	int branches = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (boardHolder[y][x] != 0 && ((boardHolder[y][x] % 2 != depth % 2 && playingBlack) || ((boardHolder[y][x] % 2 == depth % 2 && !playingBlack)))) {
				switch (boardHolder[y][x]) {
				case 1://black pawn
				case 2://white pawn
					   //moves
					   //+x
					if (x < 7 && boardHolder[y][x + 1] == 0) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//+y
					if (y < 7 && boardHolder[y + 1][x] == 0) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//-x
					if (x > 0 && boardHolder[y][x - 1] == 0) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					if (y > 0 && boardHolder[y - 1][x] == 0) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//takes
					//black peice taking
					if (y < 7 && boardHolder[y][x] == 1) {
						//taking in +x
						if (x < 7 && boardHolder[y][x] % 2 != boardHolder[y + 1][x + 1] % 2) {//if boardHolder[i][y] is other colour
							std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
							newBoard[y + 1][x + 1] = boardHolder[y][x];
							newBoard[y][x] = 0;
							routeValue += nextSetOfMoves(newBoard, depth + 1);
							branches++;
						}
						//taking in -x
						if (x > 0 && boardHolder[y][x] % 2 != boardHolder[y - 1][x + 1] % 2) {//if boardHolder[i][y] is other colour
							std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
							newBoard[y - 1][x + 1] = boardHolder[y][x];
							newBoard[y][x] = 0;
							routeValue += nextSetOfMoves(newBoard, depth + 1);
							branches++;
						}
					}
					//white peice taking
					else if (y > 0 && boardHolder[y][x] == 2) {
						//taking in +x -y
						if (x < 7 && boardHolder[y][x] % 2 != boardHolder[y - 1][x + 1] % 2) {//if boardHolder[i][y] is other colour
							std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
							newBoard[y - 1][x + 1] = boardHolder[y][x];
							newBoard[y][x] = 0;
							routeValue += nextSetOfMoves(newBoard, depth + 1);
							branches++;
						}
						//taking in -x -y
						if (x > 0 && boardHolder[y][x] % 2 != boardHolder[y - 1][x - 1] % 2) {//if boardHolder[i][y] is other colour
							std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
							newBoard[y - 1][x - 1] = boardHolder[y][x];
							newBoard[y][x] = 0;
							routeValue += nextSetOfMoves(newBoard, depth + 1);
							branches++;
						}
					}
					break;
				case 3://black rook
				case 4://white rook
					   //moving and taking in +x
					for (int i = x + 1; i < 8; i++) {
						if (boardHolder[y][i] != 0) {//if boardHolder[y][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[y][i] % 2) {//if boardHolder[y][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[y][i] % 2) {//if boardHolder[y][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[y][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +y
					for (int i = y + 1; i < 8; i++) {
						if (boardHolder[i][x] != 0) {//if boardHolder[i][x] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[i][x] % 2) {//if boardHolder[i][x] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[i][x] % 2) {//if boardHolder[i][x] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[i][x] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[i][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x
					for (int i = x - 1; i > -1; i--) {
						if (boardHolder[y][i] != 0) {//if boardHolder[y][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[y][i] % 2) {//if boardHolder[y][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[y][i] % 2) {//if boardHolder[y][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[y][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -y
					for (int i = y - 1; i > -1; i--) {
						if (boardHolder[i][x] != 0) {//if boardHolder[i][x] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[i][x] % 2) {//if boardHolder[i][x] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[i][x] % 2) {//if boardHolder[i][x] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[i][x] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[i][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					break;
				case 5://black knight
				case 6://white knight
					   //y + 1, x + 2
					if (y < 7 && x < 6 && (boardHolder[y][x] % 2 != boardHolder[y + 1][x + 2] % 2 || boardHolder[y + 1][x + 2] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x + 2] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y + 2, x + 1
					if (y < 6 && x < 7 && (boardHolder[y][x] % 2 != boardHolder[y + 2][x + 1] % 2 || boardHolder[y + 2][x + 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 2][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y + 2, x - 1
					if (y < 6 && x > 0 && (boardHolder[y][x] % 2 != boardHolder[y + 2][x - 1] % 2 || boardHolder[y + 2][x - 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 2][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y + 1, x - 2
					if (y < 7 && x > 1 && (boardHolder[y][x] % 2 != boardHolder[y + 1][x - 2] % 2 || boardHolder[y + 1][x - 2] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x - 2] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y - 1, x - 2
					if (y > 0 && x > 1 && (boardHolder[y][x] % 2 != boardHolder[y - 1][x - 2] % 2 || boardHolder[y - 1][x - 2] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x - 2] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y - 2, x - 1
					if (y > 1 && x > 0 && (boardHolder[y][x] % 2 != boardHolder[y - 2][x - 1] % 2 || boardHolder[y - 2][x - 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 2][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y - 2, x + 1
					if (y > 1 && x < 7 && (boardHolder[y][x] % 2 != boardHolder[y - 2][x + 1] % 2 || boardHolder[y - 2][x + 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 2][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//y - 1s, x + 2
					if (y > 0 && x < 6 && (boardHolder[y][x] % 2 != boardHolder[y - 1][x + 2] % 2 || boardHolder[y - 1][x + 2] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x + 2] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					break;
				case 7://black bishop
				case 8://white bishop
					   //moving and taking in +x +y
					for (int i = x + 1, t = y + 1; i < 8 && t < 8; i++, t++) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x +y
					for (int i = x - 1, t = y + 1; i > -1 && t < 8; i--, t++) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +x -y
					for (int i = x + 1, t = y - 1; i < 8 && t > -1; i++, t--) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x -y
					for (int i = x - 1, t = y - 1; i > -1 && t > -1; i--, t--) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					break;
				case 9://black queen
				case 10://white queen
						//moving and taking in +x
					for (int i = x + 1; i < 8; i++) {
						if (boardHolder[y][i] != 0) {//if boardHolder[y][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[y][i] % 2) {//if boardHolder[y][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[y][i] % 2) {//if boardHolder[y][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[y][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +y
					for (int i = y + 1; i < 8; i++) {
						if (boardHolder[i][x] != 0) {//if boardHolder[i][x] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[i][x] % 2) {//if boardHolder[i][x] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[i][x] % 2) {//if boardHolder[i][x] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[i][x] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[i][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x
					for (int i = x - 1; i > -1; i--) {
						if (boardHolder[y][i] != 0) {//if boardHolder[y][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[y][i] % 2) {//if boardHolder[y][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[y][i] % 2) {//if boardHolder[y][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[y][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -y
					for (int i = y - 1; i > -1; i--) {
						if (boardHolder[i][x] != 0) {//if boardHolder[i][x] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[i][x] % 2) {//if boardHolder[i][x] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[i][x] % 2) {//if boardHolder[i][x] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[i][x] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[i][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +x +y
					for (int i = x + 1, t = y + 1; i < 8 && t < 8; i++, t++) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x +y
					for (int i = x - 1, t = y + 1; i > -1 && t < 8; i--, t++) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +x -y
					for (int i = x + 1, t = y - 1; i < 8 && t > -1; i++, t--) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x -y
					for (int i = x - 1, t = y - 1; i > -1 && t > -1; i--, t--) {
						if (boardHolder[t][i] != 0) {//if boardHolder[t][i] is not empty
							if (boardHolder[y][x] % 2 == boardHolder[t][i] % 2) {//if boardHolder[t][i] is same colour
								break;
							}
							if (boardHolder[y][x] % 2 != boardHolder[t][i] % 2) {//if boardHolder[t][i] is other colour
								std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
								newBoard[t][i] = boardHolder[y][x];
								newBoard[y][x] = 0;
								routeValue += nextSetOfMoves(newBoard, depth + 1);
								branches++;
								break;
							}
						}
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[t][i] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					break;
				case 11://black king
				case 12://white king
						//moving and taking in +x +y
					if (y < 7 && x < 7 && (boardHolder[y][x] % 2 != boardHolder[y + 1][x + 1] % 2 || boardHolder[y + 1][x + 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +y
					if (y < 7 && (boardHolder[y][x] % 2 != boardHolder[y + 1][x] % 2 || boardHolder[y + 1][x] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x +y
					if (y < 7 && x > 0 && (boardHolder[y][x] % 2 != boardHolder[y + 1][x - 1] % 2 || boardHolder[y + 1][x - 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y + 1][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x
					if (x > 0 && (boardHolder[y][x] % 2 != boardHolder[y][x - 1] % 2 || boardHolder[y][x - 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -x -y
					if (y > 0 && x > 0 && (boardHolder[y][x] % 2 != boardHolder[y - 1][x - 1] % 2 || boardHolder[y - 1][x - 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x - 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in -y
					if (y > 0 && (boardHolder[y][x] % 2 != boardHolder[y - 1][x] % 2 || boardHolder[y - 1][x] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +x -y
					if (y > 0 && x < 7 && (boardHolder[y][x] % 2 != boardHolder[y - 1][x + 1] % 2 || boardHolder[y - 1][x + 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y - 1][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
					//moving and taking in +x
					if (x < 7 && (boardHolder[y][x] % 2 != boardHolder[y][x + 1] % 2 || boardHolder[y][x + 1] == 0)) {
						std::memcpy(newBoard, boardHolder, 8 * 8 * sizeof int());
						newBoard[y][x + 1] = boardHolder[y][x];
						newBoard[y][x] = 0;
						routeValue += nextSetOfMoves(newBoard, depth + 1);
						branches++;
					}
				}
			}
		}
	}
	//depth 1 means a move the AI could currently make
	if (depth == 1) {
		possibleMoves.push_back(findMove(boardHolder, routeValue / branches));
	}
	return routeValue / branches;
}