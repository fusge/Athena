#include "chessboard.h"

#include <iterator>
#include <string>
#include <iostream>
#include <ctype.h>

chesspeer::chessboard::chessboard () {
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	this->set_board(fen);
}

chesspeer::chessboard::chessboard (std::string fen) {
	this->set_board(fen);
}

void chesspeer::chessboard::set_board(std::string fen) {
	std::cout << "Reading the standard fen string..." << std::endl;
	std::array<char, 64>::iterator board_iter = this->board.begin();
	std::string::iterator fen_iter = fen.begin();
	int emptysquares;

	if (board.begin() == board.end()) {
		std::cout << "they are the same you dummy" << std::endl;
	}
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
	this->color_to_move = *fen_iter;
	fen_iter++;

	// Handle castling rights
	while (*fen_iter != ' ') {
		switch (*fen_iter) {
		case 'K': this->white_castle_kingside = true;
		case 'Q': this->white_castle_queenside = true;
		case 'k': this->black_castle_kingside = true;
		case 'q': this->black_castle_queenside = true;
		default: fen_iter++;
		}
	}

	// store en_passant squares
	if (*fen_iter != '-') {
		this->en_passant = std::string(fen_iter, fen_iter + 2);
		fen_iter = fen_iter + 3;
	}
	else {
		fen_iter = fen_iter + 2;
	}

	// store total moves and moves since last capture
	this->plys_since_capture = *fen_iter;
	fen_iter++;
	this->total_moves = *fen_iter;
}

void chesspeer::chessboard::show() {
	int counter = 0;
	for (auto iter = this->board.begin(); iter != this->board.end(); iter++) {
		if (counter % 8 == 0) {
			std::cout << std::endl;
		}
		std::cout << *iter << " ";
		counter++;
	}
	std::cout << std::endl;
	std::string to_move = (this->color_to_move == 'w' || this->color_to_move == 'W') ? "white" : "black";
	std::cout << to_move << " to move" << std::endl;
	std::cout << "on move " << this->total_moves << std::endl;
}

