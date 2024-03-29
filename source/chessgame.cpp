#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include "chessgame.h"

using core::empty_square;

core::chessgame::chessgame()
{
  std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  this->set_board(fen);
}

core::chessgame::chessgame(std::string fen)
{
  this->set_board(std::move(fen));
}

void core::chessgame::set_board(std::string fen)
{
  for (auto& board_row: this->m_board) {
    board_row.fill(core::empty_square);
  }

  std::cout << "Reading the standard fen string..." << std::endl;
  std::string::iterator fen_iter = fen.begin();
  int emptysquares=0;
  movenode game_node = movenode();

  // First store the board pieces
  for (int row = core::max_board_range-1; row >= 0; row--) {
    for (int col = 0; col < core::max_board_range; col++) {
      if (*fen_iter == '/') {
        fen_iter++;
      }
      if (isdigit(*fen_iter) > 0) {
        emptysquares = (*fen_iter) - '0';  // Damn you ascii;
        col += emptysquares;
        fen_iter++;
      } else {
        m_board[row][col] = static_cast<core::piece_t>(*fen_iter);
        fen_iter++;
      }
    }
  }

  // Handle whos move is it anyway
  fen_iter++;
  game_node.color_to_move = *fen_iter;
  fen_iter++;

  // Handle castling rights
  fen_iter++;
  while (*fen_iter != ' ') {
    switch (*fen_iter) {
      case core::white_king:
        game_node.white_castle_kingside = true;
        fen_iter++;
        break;
      case core::white_queen:
        game_node.white_castle_queenside = true;
        fen_iter++;
        break;
      case core::black_king:
        game_node.black_castle_kingside = true;
        fen_iter++;
        break;
      case core::black_queen:
        game_node.black_castle_queenside = true;
        fen_iter++;
        break;
    }
  }

  // store en_passant squares
  fen_iter++;
  if (*fen_iter != '-') {
    game_node.en_passant = core::coordinates(*fen_iter, *(fen_iter + 1));
    fen_iter += 3;
  } else {
    game_node.en_passant = core::coordinates(*fen_iter, *fen_iter);
    fen_iter = fen_iter + 2;
  }

  // store total moves and moves since last capture
  std::string::iterator start_digit = fen_iter;
  while (*fen_iter != ' ') {
    fen_iter++;
  }
  game_node.plys_since_capture = std::stoi(std::string(start_digit, fen_iter));

  // Store current move number
  game_node.on_move = std::stoi(std::string(fen_iter, fen.end()));

  m_game_tree.push_back(game_node);
  m_current_position_id = static_cast<uint32_t>(m_game_tree.size()) - 1;
}

void core::chessgame::show(bool flipped)
{
  if (!flipped) {
    for (int row = core::max_board_range-1; row >= 0; row--) {
      std::cout << row + 1 << " ";
      for (int col = 0; col < max_board_range; col++) {
        std::cout << '[' << this->m_board[row][col] << ']';
      }
      std::cout << std::endl;
    }
  } else {
    for (int row = 0; row < core::max_board_range; row++) {
      std::cout << row + 1 << " ";
      for (int col = max_board_range - 1; col >= 0; col--) {
        std::cout << '[' << this->m_board[row][col] << ']';
      }
      std::cout << std::endl;
    }
  }
  std::cout << "   ";
  if (!flipped) {
    for (int file = 0; file < core::max_board_range; file++) {
      std::cout << static_cast<char>(file + 'a') << "  ";
    }
  } else {
    for (int file = core::max_board_range-1; file >= 0; file--) {
      std::cout << static_cast<char>(file + 'a') << "  ";
    }
  }

  std::cout << std::endl;
  std::cout << m_game_tree[m_current_position_id].color_to_move << " to move"
            << std::endl;
  std::cout << "on move " << m_game_tree[m_current_position_id].on_move << std::endl;
}

std::string core::chessgame::get_fen()
{
  std::string fen;
  int empty_squares = 0;
  for (int row = core::max_board_range-1; row >= 0; row--) {
    for (int col = 0; col < max_board_range; col++) {
      if (this->m_board[row][col] == core::empty_square) {
        empty_squares++;
      } else {
        if (empty_squares != 0) {
          fen += static_cast<char>(empty_squares + '0');
          empty_squares = 0;
        }
        fen += std::to_string(this->m_board[row][col]);
      }
    }
    if (empty_squares != 0) {
      fen += static_cast<char>(empty_squares + '0');
      empty_squares = 0;
    }
    fen += '/';
  }

  fen += ' ';
  fen += m_game_tree[m_current_position_id].color_to_move;
  fen += ' ';

  if (m_game_tree[m_current_position_id].white_castle_kingside) {
    fen += std::to_string(core::white_king);
  }
  if (m_game_tree[m_current_position_id].white_castle_queenside) {
    fen += std::to_string(core::black_queen);
  }
  if (m_game_tree[m_current_position_id].black_castle_kingside) {
    fen += std::to_string(core::black_king);
  }
  if (m_game_tree[m_current_position_id].black_castle_queenside) {
    fen += std::to_string(core::black_queen);
  }

  fen += ' ';
  fen += m_game_tree[m_current_position_id].en_passant.file;
  if(fen.back() != '-') {
    fen += m_game_tree[m_current_position_id].en_passant.rank;
  }
  fen += ' ';

  fen += std::to_string(m_game_tree[m_current_position_id].plys_since_capture);
  fen += ' ';
  fen += std::to_string(m_game_tree[m_current_position_id].on_move);

  return fen;
}

void core::chessgame::draw_line(
    std::vector<core::coordinates>& coord_list,
    std::pair<int, int> direction,
    bool iterate)
{
  int col = coord_list.back().file - 'a' + direction.first;
  int row = coord_list.back().rank - '0' + direction.second;
  if (row < 0 || row > core::max_board_range-1 || col < 0 || col > core::max_board_range-1){
    return;
  }
  core::coordinates new_coord {' ', ' '};
  new_coord.file = static_cast<char>(coord_list.back().file + direction.first);
  new_coord.rank = static_cast<char>(coord_list.back().rank + direction.second);
  coord_list.push_back(new_coord);
  if ( iterate && this->m_board[row][col] != core::empty_square) {
    draw_line(coord_list, direction, iterate);
  }
}

std::vector<core::coordinates> core::chessgame::get_available_moves(core::coordinates square,
                                                                    char piece)
{
  std::vector<core::coordinates> result;
  std::vector<core::coordinates> coordinate_list;

  // Queen moves
  if (piece == core::black_queen || piece == core::white_queen) {
    for (int steprow = -1; steprow <= 1; steprow++) {
      for (int stepcol = -1; stepcol <= 1; stepcol++) {
        if (stepcol == 0 && steprow == 0) { continue; }
        coordinate_list.push_back(square);
        // iterate to the end of board or piece presence
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/true);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
      }
    }
  }

  // King moves
  else if (piece == core::black_king|| piece == core::white_king)
  {
    for (int steprow = -1; steprow <= 1; steprow++) {
      for (int stepcol = -1; stepcol <= 1; stepcol++) {
        if (stepcol == 0 && steprow == 0) { continue; }
        coordinate_list.push_back(square);
        // only find first squares
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/false);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
      }
    }
    // Handle castles
    if (piece == core::white_king && square == core::white_king_starting_square) {
      if (m_game_tree[m_current_position_id].white_castle_kingside
          && m_board[0][5] == core::empty_square && m_board[0][6] == core::empty_square)
      {
        square.file += 2;
        result.push_back(square);
        square.file -= 2;
      }
      if (m_game_tree[m_current_position_id].white_castle_queenside
          && m_board[0][3] == core::empty_square && m_board[0][2] == core::empty_square)
      {
        square.file -= 2;
        result.push_back(square);
        square.file += 2;
      }
    }
    if (piece == core::black_king && square == black_king_starting_square) {
      if (m_game_tree[m_current_position_id].black_castle_kingside
          && m_board[0][5] == core::empty_square && m_board[0][6] == core::empty_square)
      {
        square.file += 2;
        result.push_back(square);
        square.file -= 2;
      }
      if (m_game_tree[m_current_position_id].black_castle_queenside
          && m_board[0][3] == core::empty_square && m_board[0][2] == core::empty_square)
      {
        square.file -= 2;
        result.push_back(square);
        square.file += 2;
      }
    }
  }

  // Rook moves
  else if (piece == core::black_rook || piece == core::white_rook)
  {
    for (int steprow = -1; steprow <= 1; steprow++) {
      for (int stepcol = -1; stepcol <= 1; stepcol++) {
        if (abs(steprow) == abs(stepcol)) { continue; }
        coordinate_list.push_back(square);
        // iterate to the end of the board or piece presence.
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/true);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
      }
    }
  }

  // Bishop moves
  else if (piece == core::black_bishop || piece == core::white_bishop)
  {
    for (int steprow = -1; steprow <= 1; steprow++) {
      for (int stepcol = -1; stepcol <= 1; stepcol++) {
        if (steprow == 0 || stepcol == 0) { continue; }
        coordinate_list.push_back(square);
        // iterate to the end of the board or piece presence
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/true);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
      }
    }
  }

  // Pawn moves
  else if (piece == core::black_pawn || piece == core::white_pawn)
  {
    for (int steprow = -1; steprow <= 1; steprow++) {
      for (int stepcol = -1; stepcol <= 1; stepcol++) {
        // pawns don't move backwards or sidways. they can only move
        // diagonally for capture and en_passant moves. En passant
        // rules are dealt with at higher logic level. Here we
        // are just returning all possible moves regardless of
        // capture or en_passant status.
        if (steprow == 0 || 
            (piece == core::black_pawn && steprow > 0) || 
            (piece == core::white_pawn && steprow < 0)) {
          continue;
        }            
        coordinate_list.push_back(square);
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/false);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
       
      }
    }

    // Finally if the pawn has not moved then it can move two squares
    // forward.
    if (piece == core::black_pawn && square.rank == '7') {
      square.rank = '5';
      result.push_back(square);
    }
    if (piece == core::white_pawn && square.rank == '2') {
      square.rank = '4';
      result.push_back(square);
    }
  }

  // Knight moves
  else if (piece == core::black_knight || piece == core::white_knight)
  {
    for (int steprow = -2; steprow <= 2; steprow++) {
      for (int stepcol = -2; stepcol <= 2; stepcol++) {
        if (steprow == 0 || stepcol == 0 || abs(steprow) == abs(stepcol)) { continue; }
        coordinate_list.push_back(square);
        // iterate only once
        draw_line(coordinate_list, std::make_pair(stepcol, steprow), /*iterate=*/false);
        result.insert(result.end(), coordinate_list.begin(), coordinate_list.end());
      }
    }
  }

  return result;
}

core::piece_t core::chessgame::identify_piece(core::coordinates coord)
{
  int col = coord.file - 'a';
  int row = coord.rank - '0';
  return this->m_board[row][col];
}

core::piece_t core::chessgame::identify_piece(int index)
{
  int row = index / core::max_board_range;
  int col = index % core::max_board_range;
  return this->m_board[row][col];
}

void core::chessgame::show_possible_moves(core::coordinates square)
{
  char piece = identify_piece(square);
  if (piece == core::empty_square) {
    std::cout << "There is no piece there.\n";
    return;
  }
  std::vector<core::coordinates> choices = this->get_available_moves(square, piece);
  std::array<std::array<char, core::max_board_range>, core::max_board_range> allowedMovesBoard;
  for (int row = core::max_board_range; row >= 0; row--) {
    allowedMovesBoard[row].fill(core::empty_square);
  }
  allowedMovesBoard[square.rank - '0'][square.file - 'a'] = piece;

  int row = 0;
  int col = 0;
  for (auto choices_iter : choices)
  {
    row = choices_iter.rank - '0';
    col = choices_iter.file - 'a';
    if (allowedMovesBoard[row][col] != core::empty_square) { continue; }
    allowedMovesBoard[row][col] = 'X';
  }

  for (row = core::max_board_range - 1; row >= 0; row--) {
    for (col = 0; col < core::max_board_range; col++) {
      std::cout << '[' << allowedMovesBoard[row][col] << ']';
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

int core::chessgame::play_move(std::pair<coordinates, coordinates> move_set)
{
  // Check that we are getting the right formatted string
  //if (move_set.length() != 4) {
  //  std::cout << "Please enter move in the format: \n";
  //  std::cout << " <startfile><startrow><endfile><endrow>\n";
  //  std::cout << "e.g. e4 -> e2e4" << std::endl;
  //  return 1;
  //}
  // get source and destination strings and check for available moves
  core::coordinates start_square = move_set.first;
  core::coordinates end_square = move_set.second;
  char piece = identify_piece(start_square);
  bool match = false;
  std::vector<core::coordinates> choices = get_available_moves(start_square, piece);
  for (auto & choice : choices) {
    if (choice == end_square) {
      match = true;
      break;
    }
  }
  // return if the move doesn't make sense
  if (!match) {
    return 1;
  // check whether there is a capture (empty squares return true)
  } if (!is_valid_capture(move_set)) {
    return 1;
  }
  update_board(move_set);
  return 0;
}

bool core::chessgame::is_valid_capture(std::pair<core::coordinates, core::coordinates> move_set)
{
  core::piece_t piece = identify_piece(move_set.first);
  core::piece_t captured_piece = identify_piece(move_set.second);
  // Pawns are the only piece that changes it's move based on captures.
  if (piece == core::black_pawn || piece == core::white_pawn) {
    if (captured_piece == empty_square
        && move_set.first.file == move_set.second.file
        && abs(piece - captured_piece) > 8) {
      return true;
    } return captured_piece != core::empty_square
             && move_set.first.rank != move_set.second.rank;
  } if ((piece - captured_piece) > 8) {
    return true;
  } return captured_piece == core::empty_square;
}

core::coordinates core::chessgame::find_king(char color)
{
  unsigned int temp_node_id = m_current_position_id;
  core::piece_t piece = core::empty_square;
  core::coordinates square{' ', ' '};
  if (color == core::black_color) {
    piece = core::black_king;
    square = {'e', '8'};
  } else {
    piece = core::white_king;
    square = {'e', '1'}; 
  }

  // easiest way is to look at the moves played
  while (temp_node_id != 0) {
    if (m_game_tree[temp_node_id].pgn_move_played.front() == core::white_king
        && m_game_tree[temp_node_id].color_to_move != color)
    {
      square = m_game_tree[temp_node_id].move_played;
      break;
    }
    temp_node_id = m_game_tree[temp_node_id].prev_move_id;
  }

  // make sure that the king is where we think it is
  if (identify_piece(square) != piece) {
    for (int row = 0; row < core::max_board_range; row++) {
      for (int col = 0; col < core::max_board_range; col++) {
        if (m_board[row][col] == piece) {
          square.file = static_cast<char>(col + 'a');
          square.rank = static_cast<char>(row + '0');
        }
      }
    }
  }
  return square;
}

// TODO: finish implementation of checking for checked king
bool core::chessgame::king_in_check(std::pair<core::coordinates, core::coordinates> move_set, 
                                    char color)
{
  return false;
}

// TODO: finish implementation for pinned pieces
bool core::chessgame::piece_pinned(std::pair<core::coordinates, core::coordinates> move_set)
{
  return false;
}

void core::chessgame::update_board(std::pair<core::coordinates, core::coordinates> move_set)
{
  core::piece_t piece = identify_piece(move_set.first);
  core::coordinates start_square = move_set.first;
  core::coordinates end_square = move_set.second;
  core::coordinates move_str = move_set.second;
  movenode added_movenode = m_game_tree[m_current_position_id];
  added_movenode.captured_piece = identify_piece(move_set.second);

  // Update board
  m_board[start_square.rank - '0'][start_square.file - 'a'] = core::empty_square;
  added_movenode.captured_piece =
      m_board[end_square.rank - '0'][end_square.file - 'a'];
  m_board[end_square.rank - '0'][end_square.file - 'a'] = piece;

  // Update game state
  added_movenode.pgn_move_played.push_back(static_cast<char>(piece));
  added_movenode.pgn_move_played.push_back(move_str.file);
  added_movenode.pgn_move_played.push_back(move_str.rank);
  added_movenode.move_played = move_set.second;
  added_movenode.ply++;
  added_movenode.on_move = (added_movenode.ply / 2) + 1;
  added_movenode.color_to_move = (added_movenode.ply % 2 == 0) ? 'w' : core::black_bishop;

  if (added_movenode.captured_piece != ' ') {
    added_movenode.plys_since_capture = 0;
  } else {
    added_movenode.plys_since_capture++;
  }

  added_movenode.black_castle_queenside =
      m_game_tree[m_current_position_id].black_castle_queenside;
  added_movenode.black_castle_kingside =
      m_game_tree[m_current_position_id].black_castle_kingside;
  added_movenode.white_castle_queenside =
      m_game_tree[m_current_position_id].white_castle_queenside;
  added_movenode.white_castle_kingside =
      m_game_tree[m_current_position_id].white_castle_kingside;

  // Make sure to update en passant square
  if (piece == core::black_pawn || piece == core::white_pawn) {
    if (start_square.file == end_square.file
        && start_square.rank - end_square.rank > 1)
    {
      added_movenode.en_passant = start_square;
      // make sure en_passant square is behind the pawn
      added_movenode.en_passant.rank +=
          (end_square.rank - start_square.rank) / 2;
    }
  } else if (piece == core::black_king || piece == core::white_king) {
    added_movenode.black_castle_queenside = false;
    added_movenode.black_castle_kingside = false;
    added_movenode.white_castle_queenside = false;
    added_movenode.white_castle_kingside = false;
    // handle moving the rook if this is castles
    if (end_square.file - start_square.file == 2 && piece == core::white_king) {
      m_board[0][7] = core::empty_square;
      m_board[0][5] = core::white_rook;
    } else if (end_square.file - start_square.file == -2 && piece == core::white_king)
    {
      m_board[0][0] = core::empty_square;
      m_board[0][3] = core::white_rook;
    } else if (end_square.file - start_square.file == 2 && piece == core::black_king)
    {
      m_board[7][7] = core::empty_square;
      m_board[7][5] = core::black_rook;
    } else if (end_square.file - start_square.file == -2 && piece == core::black_king)
    {
      m_board[7][0] = core::empty_square;
      m_board[7][3] = core::black_rook;
    }
  } else if (piece == core::black_rook || piece == core::white_rook) {
    if (piece == core::white_rook && start_square == core::coordinates('a', '1')) {
      added_movenode.white_castle_queenside = false;
    }
    if (piece == core::white_rook && start_square == core::coordinates('h', '1')) {
      added_movenode.white_castle_kingside = false;
    }
    if (piece == core::black_rook && start_square == core::coordinates('a', '8')) {
      added_movenode.black_castle_queenside = false;
    }
    if (piece == core::black_rook && start_square == core::coordinates('h', '8')) {
      added_movenode.black_castle_queenside = false;
    }
  }
  added_movenode.prev_move_id = m_current_position_id;

  // We shoudl find an available slot if there is one. else append.
  int new_position_id = find_available_tree_id();
  if (new_position_id == 0) {
    m_game_tree.push_back(added_movenode);
    m_game_tree[m_current_position_id].sidelines.push_back(m_current_position_id + 1);
    m_current_position_id = static_cast<unsigned int>(m_game_tree.size()) - 1;
  } else {
    m_game_tree[new_position_id] = added_movenode;
    m_game_tree[m_current_position_id].sidelines.push_back(new_position_id);
    m_current_position_id = new_position_id;
  }
}

int core::chessgame::find_available_tree_id()
{
  unsigned int result = 0;
  for (unsigned int tree_id = 1; tree_id < m_game_tree.size(); tree_id++) {
    if (!m_game_tree[tree_id].linked) {
      result = tree_id;
      break;
    }
  }
  return result;
}

std::string core::chessgame::get_pgn()
{
  std::string pgn;
  unsigned int position_id = 0;
  while (true) {
    pgn += m_game_tree[position_id].pgn_move_played;
    pgn += " ";
    if (m_game_tree[position_id].sidelines.empty())
      break;
    position_id = m_game_tree[position_id].sidelines[0];
    if (m_game_tree[position_id].color_to_move == core::black_bishop) {
      pgn.push_back(static_cast<char>(m_game_tree[position_id].on_move + '0'));
      pgn += ".";
    }
  }
  return pgn;
}

std::string core::chessgame::generate_move_pgn(std::pair<core::coordinates, core::coordinates> move_set)
{
  std::string result;
  core::coordinates start_square = move_set.first;
  core::coordinates end_square = move_set.second;
  core::piece_t piece = identify_piece(start_square);

  // once again pawns are special
  if (piece != core::white_pawn && piece != core::black_pawn){
    result += std::to_string(piece);
  }

  // The idea now is to reverse engineer all squares where the
  // piece can come from. This is to deal with cases where we
  // have ambiguity (more than one piece can land on that square)
  std::vector<core::coordinates> available_moves_list;
  std::vector<core::coordinates> ambiguous_moves_list;
  if (piece == core::white_pawn) {
    available_moves_list = get_available_moves(end_square, core::black_pawn);
  } else if (piece == core::black_pawn) {
    available_moves_list = get_available_moves(end_square, core::white_pawn);
  } else {
    available_moves_list = get_available_moves(end_square, piece);
  }
  int count = 0;
  for (auto& iter : available_moves_list){
    if (identify_piece(iter) == piece) {
      count++;
      ambiguous_moves_list.push_back(iter);
    }
  }
  // add disambiguation symbols
  if (count > 1) {
    bool diff_row = false;
    bool diff_col = false;
    for (auto & iter : ambiguous_moves_list)
    {
      if (start_square != iter) {
        diff_row = (start_square.rank != iter.rank);
        diff_col = (start_square.file != iter.file);
      }
    }
    if (diff_col) {
      result += std::to_string(start_square.file);
    }
    if (diff_row) {
      result += std::to_string(start_square.rank);
    }
  }

  // Add captures symbol
  if (identify_piece(end_square) != core::empty_square) {
    result += 'x';
  }

  result += std::to_string(end_square.file);
  result += std::to_string(end_square.rank);

  return result;
}

