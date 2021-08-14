#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <vector>
#include <array>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <memory>

namespace chesspeer {
	struct Movenode {
		std::string value;
		int ply;
		char color_to_move;
		char captured_piece;
		std::string en_passant;
		bool black_castle_queenside;
		bool black_castle_kingside;
		bool white_castle_queenside;
		bool white_castle_kingside;
		int plys_since_capture;
		int on_move;
        std::string move_played;
		struct Movenode* prevmove;
		std::vector<std::shared_ptr<Movenode>> sidelines;
	};

	class chessgame {
	private:
		std::array<std::array<char, 8>, 8> board;
		std::shared_ptr<Movenode> gameTree;
		std::shared_ptr<Movenode> currentPosition;

		void _drawLine(std::shared_ptr<std::list<std::string>> coordinates, std::pair<int, int> direction, bool iterate);
		std::list<std::string> _availableMoves(std::string square, char piece);

	public:
		chessgame();
		chessgame(std::string fen);

		std::string get_fen();
		void set_board(std::string fen);
		void play_move(std::string move);
		std::vector<std::string> get_moves();
        char identifyPiece(std::string square);

		void show(bool fliped);
        void showPossibleMoves(std::string square);
	};
}

#endif
