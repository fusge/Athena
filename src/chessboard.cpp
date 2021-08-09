#include "chessboard.h"

#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cmath>

chesspeer::chessboard::chessboard () {
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	this->set_board(fen);
}

chesspeer::chessboard::chessboard (std::string fen) {
	this->set_board(fen);
}

void chesspeer::chessboard::set_board(std::string fen) {
	for (int row; row < 8; row++) {
		this->board[row].fill(' ');
	}

	std::cout << "Reading the standard fen string..." << std::endl;
	std::string::iterator fen_iter = fen.begin();
	int emptysquares;
	struct chesspeer::Movenode* gameTree = new chesspeer::Movenode();

	// First store the board pieces
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (*fen_iter == '/') {
				fen_iter++;
				row = row + 1;
				col = 8;
			}
			if (isdigit(*fen_iter)) {
				emptysquares = (*fen_iter) - 48; // Damn you ascii;
				col = col + emptysquares - 1;
				fen_iter++;
			}
			else {
				this->board[row][col] = *fen_iter;
				fen_iter++;
			}
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
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (this->board[row][col] == ' ') {
				std::cout << char(254) << " ";
			}
			else {
				std::cout << this->board[row][col] << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << this->gameTree->color_to_move << " to move" << std::endl;
	std::cout << "on move " << this->gameTree->on_move << std::endl;
}

std::string chesspeer::chessboard::get_fen() {
	std::string fen = "";
	int empty_squares = 0;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (this->board[row][col] == ' ') {
				empty_squares++;
			}
			else {
				if (empty_squares != 0) {
					fen += char(empty_squares + 48);
					empty_squares = 0;
				}
				fen += this->board[row][col];
			}
		}
		if (empty_squares != 0) {
			fen += char(empty_squares + 48);
		}
		if (row != 7) {
			fen += '/';
		}
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

void chesspeer::chessboard::_drawLine(std::list<std::string> coordinates, std::pair<int, int> direction, bool iterate) {
	int row = int(coordinates.back()[0]) - 48;
	int col = int(coordinates.back()[1]) - 97;
	if (row < 0 || row > 7 || col < 0 || col > 7) return;
	if (this->board[row][col] != ' ') return;
	std::string new_coord;
	new_coord.push_back(coordinates.back()[0] + direction.first);
	new_coord.push_back(coordinates.back()[1] + direction.second);
	coordinates.push_back(new_coord);
	if (!iterate) return;
	this->_drawLine(coordinates, direction, iterate);
}

std::list<std::string> chesspeer::chessboard::_availableMoves(std::string square, char piece) {
	std::list<std::string> result;
	std::list<std::string> coordinates;

	// Queen moves
	if (piece == 'q' || piece == 'Q') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (stepcol == 0 && steprow == 0) continue;
				coordinates = { square };
				// iterate to the end of board or piece presence
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.merge(coordinates);
			}
		}
	}

	// King moves
	else if (piece == 'k' || piece == 'K') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (stepcol == 0 && steprow == 0) continue;
				coordinates = { square };
				// only find first squares
				_drawLine(coordinates, std::make_pair(stepcol, steprow), false);
				result.merge(coordinates);
			}
		}
	}

	// Rook moves
	else if (piece == 'r' || piece == 'R') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (abs(stepcol) == abs(stepcol)) continue;
				coordinates = { square };
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.merge(coordinates);
			}
		}
	}

	// Bishop moves
	else if (piece == 'b' || piece == 'B') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (steprow == 0 || stepcol == 0) continue;
				coordinates = { square };
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.merge(coordinates);
			}
		}
	}

	// Pawn moves
	else if (piece == 'p' || piece == 'P') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if () continue;
				coordinates = { square };
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.merge(coordinates);
			}
		}
	}

	return result;
}


