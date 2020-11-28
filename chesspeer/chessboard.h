#ifndef CHESSPEER_H
#define CHESSPEER_H

#include <vector>
#include <array>
#include <string>
#include <map>

namespace chesspeer {
	struct Movenode {
		std::string value;
		unsigned int ply;
		char color_to_move;
		char captured_piece;
		std::string en_passant;
		bool black_castle_queenside;
		bool black_castle_kingside;
		bool white_castle_queenside;
		bool white_castle_kingside;
		int plys_since_capture;
		int on_move;
		struct Movenode* prevmove;
		struct Movenode* nextmove;
		std::map<std::string,struct Movenode*> sidelines;
	};

	class chessboard {
	private:
		std::array<char, 64> board;
		struct Movenode* current_move;

	public:
		chessboard ();
		chessboard (std::string fen);

		std::string get_fen();
		void set_board(std::string fen);
		void set_pieces_style();
		// std::vector<std::string> get_moves();

		void show();
	};
}

#endif /* CHESSPEER_H */