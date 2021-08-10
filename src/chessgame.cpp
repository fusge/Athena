#include "chessgame.h"

#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cmath>

chesspeer::chessgame::chessgame() {
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	this->set_board(fen);
}

chesspeer::chessgame::chessgame(std::string fen) {
	this->set_board(fen);
}

void chesspeer::chessgame::set_board(std::string fen) {
	for (int row = 0; row < 8; row++) {
		this->board[row].fill(' ');
	}

	std::cout << "Reading the standard fen string..." << std::endl;
	std::string::iterator fen_iter = fen.begin();
	int emptysquares;
	struct chesspeer::Movenode* gameTree = new chesspeer::Movenode();

	// First store the board pieces
	for (int row = 7; row >= 0; row--) {
		for (int col = 0; col < 8; col++) {
			if (*fen_iter == '/') {
				fen_iter++;
			}
			if (isdigit(*fen_iter)) {
				emptysquares = (*fen_iter) - 48; // Damn you ascii;
				col = col + emptysquares;
				fen_iter++;
			}
			else {
				board[row][col] = *fen_iter;
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
	std::string::iterator start_digit = fen_iter;
	while (*fen_iter != ' ') {
		fen_iter++;
	}
	gameTree->plys_since_capture = std::stoi(std::string(start_digit, fen_iter));

	// Store current move number
	gameTree->on_move = std::stoi(std::string(fen_iter, fen.end()));

	this->gameTree = gameTree;
}

void chesspeer::chessgame::show() {
	for (int row = 7; row >= 0; row--) {
		for (int col = 0; col < 8; col++) {
			if (board[row][col] == ' ') {
				std::cout << "[ ]";
			}
			else {
				std::cout << '[' << this->board[row][col] << ']';
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << this->gameTree->color_to_move << " to move" << std::endl;
	std::cout << "on move " << this->gameTree->on_move << std::endl;
}

std::string chesspeer::chessgame::get_fen() {
	std::string fen = "";
	int empty_squares = 0;
	for (int row = 7; row >= 0; row--){
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
            empty_squares = 0;
		}
	    fen += '/';
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

	fen += std::to_string(this->gameTree->plys_since_capture);
	fen += ' ';
	fen += std::to_string(this->gameTree->on_move);
	

	return fen;
}

void chesspeer::chessgame::_drawLine(std::list<std::string> *coordinates, std::pair<int, int> direction, bool iterate) {
	int row = int(coordinates->back()[0]) - 97;
	int col = int(coordinates->back()[1]) - 48;
	if (row < 0 || row > 7 || col < 0 || col > 7) return;
	std::string new_coord;
	new_coord.push_back(coordinates->back()[0] + direction.first);
	new_coord.push_back(coordinates->back()[1] + direction.second);
	coordinates->push_back(new_coord);
	if (!iterate) return;
	if (this->board[row][col] != ' ') return;
	this->_drawLine(coordinates, direction, iterate);
}

std::list<std::string> chesspeer::chessgame::_availableMoves(std::string square, char piece) {
	std::list<std::string> result;
	std::list<std::string>* coordinates{};

	// Queen moves
	if (piece == 'q' || piece == 'Q') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (stepcol == 0 && steprow == 0) continue;
				coordinates->push_back(square);
				// iterate to the end of board or piece presence
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.splice(result.end(), *coordinates);
			}
		}
	}

	// King moves
	else if (piece == 'k' || piece == 'K') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (stepcol == 0 && steprow == 0) continue;
				coordinates->push_back(square);
				// only find first squares
				_drawLine(coordinates, std::make_pair(stepcol, steprow), false);
				// Castling is so complex it will be dealt at higher level
				// using the state information from chessgame class.
				result.splice(result.end(), *coordinates);
			}
		}
	}

	// Rook moves
	else if (piece == 'r' || piece == 'R') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (abs(stepcol) == abs(stepcol)) continue;
				coordinates->push_back(square);
				// iterate to the end of the board or piece presence.
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.splice(result.end(), *coordinates);
			}
		}
	}

	// Bishop moves
	else if (piece == 'b' || piece == 'B') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				if (steprow == 0 || stepcol == 0) continue;
				coordinates->push_back(square);
				// iterate to the end of the board or piece presence
				_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
				result.splice(result.end(), *coordinates);
			}
		}
	}

	// Pawn moves
	else if (piece == 'p' || piece == 'P') {
		for (int steprow = -1; steprow <= 1; steprow++) {
			for (int stepcol = -1; stepcol <= 1; stepcol++) {
				// pawns don't move backwards or sidways. they can only move 
				// diagonally for capture and en_passant moves. those complex
				// rules are dealt with at higher logic level. We only want
				// the possible squares to move to.
				if (steprow == 0) continue;
				else if (piece == 'p' && steprow > 0) continue;
				else if (piece == 'P' && steprow < 0) continue;
				else {
					coordinates->push_back(square);
					_drawLine(coordinates, std::make_pair(stepcol, steprow), true);
					result.splice(result.end(), *coordinates);
				}
			}
		}

		// Finally if the pawn has not moved then it can move two squares
		// forward.
		if (piece == 'p' && square[1] == '7')
			result.push_back(square.replace(1, 1, "5"));
		if (piece == 'P' && square[1] == '2')
			result.push_back(square.replace(1, 1, "4"));
	}

	// Knight moves
	else if (piece == 'n' || piece == 'N') {
		for (int steprow = -2; steprow <= 2; steprow++) {
			for (int stepcol = -2; stepcol <= 2; stepcol++) {
				if (steprow == 0 || stepcol == 0) continue;
				else if (abs(steprow) == abs(stepcol)) continue;
				else {
					coordinates->push_back(square);
					// iterate only once
					_drawLine(coordinates, std::make_pair(stepcol, steprow), false);
					result.splice(result.end(), *coordinates);
				}
			}
		}
	}

	return result;
}

void play_move(std::string move){

}
