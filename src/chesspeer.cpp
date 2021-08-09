// chesspeer.cpp : Defines the entry point for the application.
//

#include "chesspeer.h"
#include "chessgame.h"

int main()
{
	auto mychessboard = chesspeer::chessgame();
	mychessboard.show();
	std::cout << mychessboard.get_fen() << std::endl;
}
