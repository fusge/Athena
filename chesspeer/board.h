#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace chess
{
	class board
	{
	public:
		board();
		~board();
		void move(std::string ply);
		print();

	private:
		bool validposition(std::string fen);
		bool validmove(std::string ply);
		char* chessboard[8][8];
	};
}