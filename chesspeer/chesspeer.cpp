// chesspeer.cpp : Defines the entry point for the application.
//

#include "chesspeer.h"
#include "chessboard.h"

int main()
{
	auto mychessboard = chesspeer::chessboard();
    std::string moves;
    char piece;

	mychessboard.show();
	std::cout << mychessboard.get_fen() << std::endl;
    while(true){
        std::cout << "Type in move squares: " << std::endl;
        std::cin >> moves;
        std::cout << "type in piece: " << std::endl;
        std::cin >> piece;
    }
}
