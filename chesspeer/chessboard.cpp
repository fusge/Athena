#include "chessboard.h"

#include <iterator>
#include <string>
#include <iostream>
#include <ctype>

chesspeer::chessboard::chessboard () {
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	this->set_board(fen);
}

chesspeer::chessboard::chessboard (std::string fen) {
	this->set_board(fen);
}

void chesspeer::chessboard::set_board(std::string fen) {
	this->board.fill(' ');
    int row = 0;
    int col = 0;
	std::cout << "Reading the standard fen string..." << std::endl;
	std::array<char, 64>::iterator board_iter = this->board.begin();
	//std::string::iterator fen_iter = fen.begin();
	int emptysquares;
	struct chesspeer::Movenode* gameTree = new chesspeer::Movenode();

	// First store the board pieces
	for(int board_indx=0; board_indx < 64; board_indx++) {
		if (*fen_iter == '/') {
			fen_iter++;
            board_index--;
		}
		else if (isdigit(*fen_iter)) {
			board_indx = boardindex + (*fen_iter) - 1;
			fen_iter++;
		}
		else {
            row = board_indx / 8;
            col = board_indx % 8;
			this->board[row][col] = *fen_iter;
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

	std::string nums;
	nums = this->gameTree->plys_since_capture;
	fen.append(nums);
	fen += ' ';
	nums = this->gameTree->on_move10;
	fen.append(nums);

	return fen;
}

