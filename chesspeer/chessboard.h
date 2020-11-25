#ifndef CHESSPEER_H
#define CHESSPEER_H

#include <vector>
#include <array>
#include <string>

namespace chesspeer {
	class chessboard {
	private:
		std::array<char, 64> board;
		std::vector<std::string> moves;
		char color_to_move;
		bool black_castle_queenside;
		bool black_castle_kingside;
		bool white_castle_queenside;
		bool white_castle_kingside;
		std::string en_passant;
		int plys_since_capture;
		int total_moves;

	public:
		chessboard ();
		chessboard (std::string fen);

		std::string get_fen();
		void set_board(std::string fen);
		std::vector<std::string> get_moves();

		void show();
	};
}

#endif /* CHESSPEER_H */