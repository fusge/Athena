// chesspeer.cpp : Defines the entry point for the application.
//

//#include "chesspeer.h"
#include "chessgame.h"
#include "src/ChesspeerConfig.h"
#include <iostream>

int main(int argc, char *argv[]){
    std::cout << argv[0] << " VERSION " ;
    std::cout << CHESSPEER_VERSION_MAJOR << ".";
    std::cout << CHESSPEER_VERSION_MINOR << ".";
    std::cout << CHESSPEER_VERSION_PATCH << std::endl;

	auto mychessboard = chesspeer::chessgame();
	mychessboard.show();
	std::cout << mychessboard.get_fen() << std::endl;

    std::cout << "Testing movesets for core" << std::endl;
    char piece;
    std::string test_fen = "8/8/8/8/3P4/8/8/8 w KQkq - 0 1";
    while (true){
        std::cout << "Please enter a piece" << std::endl;
        std::cin >> piece;
        test_fen[9] = piece;
        mychessboard.set_board(test_fen);
        mychessboard.showPossibleMoves("e4");
    }
}
