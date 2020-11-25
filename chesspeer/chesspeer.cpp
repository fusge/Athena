// chesspeer.cpp : Defines the entry point for the application.
//

#include "chesspeer.h"
#include "chessboard.h"

int main()
{
	auto mychessboard = chesspeer::chessboard();
	mychessboard.show();
}
