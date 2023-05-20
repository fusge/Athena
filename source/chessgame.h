#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <cstddef>
#include <vector>
#include <array>
#include <string>
#include <list>
#include <utility>
#include <memory>

namespace core {

using coordinates = struct coordinates {
  coordinates(char column, char row) : file(column), rank(row) {} 
  char file;
  char rank;
  bool operator==(const coordinates& rhs) const {
    return (this->file == rhs.file) && (this->rank == rhs.rank);
  }
  bool operator!=(const coordinates& rhs) const {
    return !operator==(rhs);
  }
};

struct movenode {
  bool linked = true;
  int ply = 0;
  char color_to_move = 'w';
  char captured_piece = ' ';
  coordinates en_passant = {'-', '-'};
  bool black_castle_queenside = false;
  bool black_castle_kingside = false;
  bool white_castle_queenside = false;
  bool white_castle_kingside = false;
  int plys_since_capture = 0;
  int on_move = 0;
  std::string pgn_move_played;
  coordinates move_played = {'-', '-'};
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

constexpr int max_board_range = 8;
constexpr char white_color = 'w';
constexpr char black_color = 'b';
const coordinates white_king_starting_square {'e','1'};
const coordinates black_king_starting_square {'e','8'};
class chessgame {
  public:
  chessgame();
  explicit chessgame(std::string fen);

  std::string get_fen();
  std::string get_pgn();
  void set_board(std::string fen);
  std::string generate_move_pgn(std::pair<coordinates, coordinates> move_set);
  int play_move(std::pair<coordinates, coordinates> move_set);
  core::piece_t identify_piece(coordinates coord);
  core::piece_t identify_piece(int index);

  void show(bool flipped);
  void show_possible_moves(coordinates square);

  private:
  std::array<std::array<core::piece_t , max_board_range>, max_board_range> m_board;
  std::vector<movenode> m_game_tree;
  uint32_t m_current_position_id;

  void draw_line(std::vector<core::coordinates>& coord_list, std::pair<int, int> direction, bool iterate);
  std::vector<coordinates> get_available_moves(coordinates square, char piece);
  bool is_valid_capture(std::pair<coordinates, coordinates> move_set);
  bool king_in_check(std::pair<coordinates, coordinates> move_set, char color);
  bool piece_pinned(std::pair<coordinates, coordinates> move_set);
  void update_board(std::pair<coordinates, coordinates> move_set);
  int find_available_tree_id();
  core::coordinates find_king(char color);

};
} // namespace core

#endif
