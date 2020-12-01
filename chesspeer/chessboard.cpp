#include "chessboard.h"

#include <iterator>
#include <string>
#include <iostream>
#include <cstdlib>

chesspeer::chessboard::chessboard () {
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	this->set_board(fen);
}

chesspeer::chessboard::chessboard (std::string fen) {
	this->set_board(fen);
}

void chesspeer::chessboard::set_board(std::string fen) {
	this->board.fill(' ');
	std::cout << "Reading the standard fen string..." << std::endl;
	std::array<char, 64>::iterator board_iter = this->board.begin();
	std::string::iterator fen_iter = fen.begin();
	int emptysquares;
	struct chesspeer::Movenode* gameTree = new chesspeer::Movenode();

	// First store the board pieces
	for(board_iter; board_iter != this->board.end(); board_iter++) {
		if (*fen_iter == '/') {
			fen_iter++;
		}
		if (isdigit(*fen_iter)) {
			emptysquares = (*fen_iter) - 48; // Damn you ascii;
			board_iter = board_iter + emptysquares - 1;
			fen_iter++;
		}
		else {
			*board_iter = *fen_iter;
			fen_iter++;
		}
	}

	// Handle whos move is it anyway
	fen_iter++;
	gameTree->color_to_move = *fen_iter;
	fen_iter++;

	// Handle castling rights
	fen_iter++;
	while (*fen_iter != ' ') {
		switch (*fen_iter) {
		case 'K': gameTree->white_castle_kingside = true;
		case 'Q': gameTree->white_castle_queenside = true;
		case 'k': gameTree->black_castle_kingside = true;
		case 'q': gameTree->black_castle_queenside = true;
		default: fen_iter++;
		}
	}

	// store en_passant squares
	fen_iter++;
	if (*fen_iter != '-') {
		gameTree->en_passant = std::string(fen_iter, fen_iter + 2);
		fen_iter = fen_iter + 3;
	}
	else {
		gameTree->en_passant = *fen_iter;
		fen_iter = fen_iter + 2;
	}

	// store total moves and moves since last capture
	auto start_digit = fen_iter;
	while (*fen_iter != ' ') {
		fen_iter++;
	}
	gameTree->plys_since_capture = std::stoi(std::string(start_digit, fen_iter));

	fen_iter++;
	gameTree->on_move = std::stoi(std::string(fen_iter, fen.end()));

	this->gameTree = gameTree;
}

void chesspeer::chessboard::show() {
	int counter = 0;
	for (auto iter = this->board.begin(); iter != this->board.end(); iter++) {
		if (counter % 8 == 0) {
			std::cout << std::endl;
		}
		if (*iter == ' ') {
			std::cout << char(254) << " ";
		}
		else {
			std::cout << *iter << " ";
		}
		counter++;
	}
	std::cout << std::endl;
	std::cout << this->gameTree->color_to_move << " to move" << std::endl;
	std::cout << "on move " << this->gameTree->on_move << std::endl;
}

std::string chesspeer::chessboard::get_fen() {
	std::string fen = "";
	int col_counter = 0;
	int empty_squares = 0;
	for (auto board_iter = this->board.begin(); board_iter != this->board.end(); board_iter++) {
		if ((col_counter % 8) == 0 && (col_counter != 0)) {
			if (empty_squares != 0) {
				fen += char(empty_squares + 48);
				empty_squares = 0;
			}
			fen.append("/");
		}
		if (*board_iter == ' ') {
			empty_squares++;
			col_counter++;
		}
		else {
			if (empty_squares != 0) {
				fen += char(empty_squares + 48);
				empty_squares = 0;
			}
			fen += *board_iter;
			col_counter++;
		}
	}
	if (empty_squares != 0) {
		fen += char(empty_squares + 48);
	}

	fen += ' ';
	fen += this->gameTree->color_to_move;
	fen += ' ';

	if (this->gameTree->white_castle_kingside == true) {
		fen += 'K';
	}
	if (this->gameTree->white_castle_queenside == true) {
		fen += 'Q';
	}
	if (this->gameTree->black_castle_kingside == true) {
		fen += 'k';
	}
	if (this->gameTree->black_castle_queenside == true) {
		fen += 'q';
	}

	fen += ' ';
	fen += this->gameTree->en_passant;
	fen += ' ';

	char nums[4];
	_itoa(this->gameTree->plys_since_capture, nums, 10);
	fen.append(nums);
	fen += ' ';
	_itoa(this->gameTree->on_move, nums, 10);
	fen.append(nums);

	return fen;
}

bool chesspeer::chessboard::_checkbounds(int row, int col) {
	return ((row >= 0) && (row < 8) && (col >= 0) && (col < 8));
}

int chesspeer::chessboard::_coordtoindex(std::string coordinates) {
	int col = int(coordinates[0]) - 97;
	int row = 8 - (int(coordinates[1]) - 48);
	int boardindex = (row * 8) + col;
	return boardindex;
}

std::vector<int> chesspeer::chessboard::_knightMoves(std::string square) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	for (int rowdiff = -2; rowdiff <= 2; rowdiff++) {
		for (int coldiff = -2; coldiff <= 2; coldiff++) {
			if ((coldiff != rowdiff) && (abs(coldiff) != abs(rowdiff)) && (coldiff != 0 || rowdiff != 0)) {
				if (this->_checkbounds(row+rowdiff, col+coldiff)) {
					indices.push_back(((row + rowdiff) * 8) + (col + coldiff));
				}
			}
		}
	}
	return indices;
}

std::vector<int> chesspeer::chessboard::_kingMoves(std::string square) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	for (int rowdiff = -1; rowdiff <= 1; rowdiff++) {
		for (int coldiff = -1; coldiff <= 1; coldiff++) {
			if ((coldiff != 0) || (rowdiff != 0)) {
				if (this->_checkbounds(row+rowdiff, col+coldiff)) {
					indices.push_back(((row + rowdiff) * 8) + (col + coldiff));
				}
			}
		}
	}
	return indices;
}

std::vector<int> chesspeer::chessboard::_bishopMoves(std::string square) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	for (int rowdiff = -7; rowdiff <= 7; rowdiff++) {
		if (rowdiff != 0) {
			if (this->_checkbounds(row+rowdiff, col+rowdiff)) {
				indices.push_back(((row + rowdiff) * 8) + (col + rowdiff));
			}
		}
	}
	return indices;
}

std::vector<int> chesspeer::chessboard::_queenMoves(std::string square) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	for (int rowdiff = -7; rowdiff <= 7; rowdiff++) {
		for (int coldiff_mult = -1; coldiff_mult <= 1; coldiff_mult++) {
			if (rowdiff != 0 || coldiff_mult != 0) {
				if (this->_checkbounds(row+rowdiff, col+(rowdiff*coldiff_mult))) {
					indices.push_back(((row + rowdiff) * 8) + (col + (rowdiff * coldiff_mult)));
				}
			}
		}
	}
	return indices;
}

std::vector<int> chesspeer::chessboard::_rookMoves(std::string square) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	for (int indxdiff = -7; indxdiff <= 7; indxdiff++) {
		if (indxdiff != 0 ) {
			if (this->_checkbounds(row+indxdiff, col)) {
				indices.push_back(((row + indxdiff) * 8) + col);
			}
			if (this->_checkbounds(row, col+indxdiff)) {
				indices.push_back((row * 8) + (col + indxdiff));
			}
		}
	}
	return indices;
}

std::vector<int> chesspeer::chessboard::_pawnMoves(std::string square, char color) {
	int index = this->_coordtoindex(square);
	std::vector<int> indices;
	int row = index / 8;
	int col = index % 8;
	if (color == 'w') {
		indices.push_back(((row - 1) * 8) + col);
		if (this->_checkbounds(row - 1, col+1)) {
			indices.push_back(((row - 1) * 8) + col + 1);
		}
		if (this->_checkbounds(row - 1, col - 1)) {
			indices.push_back(((row - 1) * 8) + col - 1);
		}
	}
	else if (color == 'b') {
		indices.push_back(((row + 1) * 8) + col);
		if (this->_checkbounds(row + 1, col + 1)) {
			indices.push_back(((row + 1) * 8) + col + 1);
		}
		if (this->_checkbounds(row + 1, col - 1)) {
			indices.push_back(((row + 1) * 8) + col - 1);
		}
	}
	return indices;
}

bool chesspeer::chessboard::isValidMove(std::string move) {
	std::string::iterator iter = move.begin();
	
}

