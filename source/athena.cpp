// chesspeer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "chessgame.h"
#include "AthenaConfig.h"

int main() {
  std::cout << "Chesspeer VERSION " ;
  std::cout << athena_version_major << ".";
  std::cout << athena_version_minor << ".";
  std::cout << athena_version_patch << std::endl;

  auto mychessboard = core::chessgame();
  std::string moves;
  while(true) {
    std::cout << "Enter starting square: ";
    std::cin >> moves;
    if(moves.size() != 4){
      std::cout << "Please enter move in the format: \n";
      std::cout << " <startfile><startrow><endfile><endrow>\n";
      std::cout << "e.g. e4 -> e2e4" << std::endl;
      continue;
    }
    mychessboard.play_move({{moves[0], moves[1]}, {moves[2], moves[3]}});
    mychessboard.show(/*flipped=*/false);
    std::cout << mychessboard.get_pgn() << std::endl;
  }
}
