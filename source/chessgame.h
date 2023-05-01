#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <vector>
#include <array>
#include <string>
#include <list>
#include <utility>
#include <memory>

namespace core {
struct movenode {
  bool linked = true;
  int ply = 0;
  char color_to_move = 'w';
  char captured_piece = ' ';
  std::string en_passant;
  bool black_castle_queenside = false;
  bool black_castle_kingside = false;
  bool white_castle_queenside = false;
  bool white_castle_kingside = false;
  int plys_since_capture = 0;
  int on_move = 0;
  std::string pgn_move_played;
  std::string move_played;
  int prev_move_id = 0;
  std::vector<int> sidelines;
};

enum piece_t {
  white_pawn = 'P',
  white_knight = 'N',
  white_bishop = 'B',
  white_rook = 'R',
  white_queen = 'Q',
  white_king = 'K',
  black_pawn = 'p',
  black_knight = 'n',
  black_bishop = 'b',
  black_rook = 'r',
  black_queen = 'q',
  black_king = 'k',
  empty_square = ' '
};

class chessgame {
  public:
  chessgame();
  explicit chessgame(std::string fen) ;

  std::string get_fen();
  std::string get_pgn();
  void set_board(std::string fen);
  std::string generate_move_pgn(std::string move_set);
  int play_move(std::string move_set);
  char identify_piece(std::string square);
  char identify_piece(int index);

  void show(bool fliped);
  void show_possible_moves(std::string square);

  private:
  std::array<std::array<char, 8>, 8> board;
  std::vector<movenode> m_game_tree;
  unsigned int m_current_position_id;

  void draw_line(std::shared_ptr<std::vector<std::string> > coordinates, std::pair<int, int> direction, bool iterate);
  std::vector<std::string> get_available_moves(std::string square, char piece);
  bool is_valid_capture(std::string move_set);
  bool king_in_check(std::string move_set, char color);
  bool piece_pinned(std::string move_set);
  void update_board(std::string move_set);
  int find_available_tree_id();
  std::string find_king(char color);

};
} // namespace core

#endif
