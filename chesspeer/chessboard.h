#include <iostream>
#include <vector>
#include <string>

namespace chesspeer {
	class chessboard {
		vector<char> board(64);
	public:
		board();
		~board();

		std::string get_fen();
		void set_board(std::string fen);
		void print();

	};
}