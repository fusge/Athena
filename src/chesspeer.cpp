// chesspeer.cpp : Defines the entry point for the application.
//

//#include "chesspeer.h"
#include "chessgame.h"
#include "src/ChesspeerConfig.h"
#include <iostream>

int main(int argc, char *argv[]){
    std::cout << "Chesspeer VERSION " ;
    std::cout << CHESSPEER_VERSION_MAJOR << ".";
    std::cout << CHESSPEER_VERSION_MINOR << ".";
    std::cout << CHESSPEER_VERSION_PATCH << std::endl;

    auto mychessboard = chesspeer::chessgame();
    std::string moves;
    while(true){
        std::cout << "Enter a square: ";
        std::cin >> moves;
        mychessboard.showPossibleMoves(moves);
    }
    mychessboard.show(false);
    //std::cout << mychessboard.getFen() << std::endl;
}
