#include <cmath>
#include <cstdlib>
#include <iostream>

#include "chessgame.h"

core::chessgame::chessgame()
    : m_board()
    , m_current_position_id(1)
{
  const std::string fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  this->set_board(fen);
}

core::chessgame::chessgame(std::string fen)
    : m_board()
    , m_current_position_id(0)
{
  this->set_board(std::move(fen));
}

void core::chessgame::set_board(std::string fen)
{
  for (auto& board_row : this->m_board) {
    board_row.fill(core::empty_square);
  }

  std::cout << "Reading the standard fen string...\n";
  std::string::iterator fen_iter = fen.begin();
  int emptysquares = 0;
  movenode game_node = movenode();

  // First store the board pieces
  for (int row = core::max_board_range - 1; row >= 0; row--) {
    for (size_t col = 0; col < core::max_board_range; col++) {
      if (*fen_iter == '/') {
        fen_iter++;
      }
      if (isdigit(*fen_iter) > 0) {
        emptysquares = (*fen_iter) - '1';  // Damn you ascii;
        col += static_cast<size_t>(emptysquares);
        fen_iter++;
      } else {
        m_board.at(static_cast<size_t>(row)).at(col) =
            static_cast<core::piece_t>(*fen_iter);
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
  const std::string::iterator start_digit = fen_iter;
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
    for (int row = core::max_board_range - 1; row >= 0; row--) {
      std::cout << row + 1 << " ";
      for (size_t col = 0; col < max_board_range; col++) {
        std::cout << '[' << this->m_board.at(static_cast<size_t>(row)).at(col)
                  << ']';
      }
      std::cout << "\n";
    }
  } else {
    for (size_t row = 0; row < core::max_board_range; row++) {
      std::cout << row + 1 << " ";
      for (int col = max_board_range - 1; col >= 0; col--) {
        std::cout << '[' << this->m_board.at(row).at(static_cast<size_t>(col))
                  << ']';
      }
      std::cout << "\n";
    }
  }
  std::cout << "   ";
  if (!flipped) {
    for (int file = 0; file < core::max_board_range; file++) {
      std::cout << static_cast<char>(file + 'a') << "  ";
    }
  } else {
    for (int file = core::max_board_range - 1; file >= 0; file--) {
      std::cout << static_cast<char>(file + 'a') << "  ";
    }
  }

  std::cout << "\n";
  std::cout << m_game_tree[m_current_position_id].color_to_move << " to move"
            << "\n";
  std::cout << "on move " << m_game_tree[m_current_position_id].on_move << "\n";
}

std::string core::chessgame::get_fen()
{
  std::string fen;
  int empty_squares = 0;
  for (int row = core::max_board_range - 1; row >= 0; row--) {
    for (size_t col = 0; col < max_board_range; col++) {
      if (this->m_board.at(static_cast<size_t>(row)).at(col)
          == core::empty_square)
      {
        empty_squares++;
      } else {
        if (empty_squares != 0) {
          fen += static_cast<char>(empty_squares + '1');
          empty_squares = 0;
        }
        fen +=
            std::to_string(this->m_board.at(static_cast<size_t>(row)).at(col));
      }
    }
    if (empty_squares != 0) {
      fen += static_cast<char>(empty_squares + '1');
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
  if (fen.back() != '-') {
    fen += m_game_tree[m_current_position_id].en_passant.rank;
  }
  fen += ' ';

  fen += std::to_string(m_game_tree[m_current_position_id].plys_since_capture);
  fen += ' ';
  fen += std::to_string(m_game_tree[m_current_position_id].on_move);

  return fen;
}

void core::chessgame::draw_line(std::vector<core::coordinates>& coord_list,
                                std::pair<int, int> direction,
                                bool iterate)
{
  const int col = coord_list.back().file - 'a' + direction.first;
  const int row = coord_list.back().rank - '1' + direction.second;
  if (row < 0 || row > core::max_board_range - 1 || col < 0
      || col > core::max_board_range - 1)
  {
    return;
  }
  core::coordinates new_coord {' ', ' '};
  new_coord.file = static_cast<char>(coord_list.back().file + direction.first);
  new_coord.rank = static_cast<char>(coord_list.back().rank + direction.second);
  coord_list.push_back(new_coord);
  if (iterate
      && this->m_board.at(static_cast<size_t>(row)).at(static_cast<size_t>(col))
          != core::empty_square)
  {
    draw_line(coord_list, direction, iterate);
  }
}

std::vector<core::coordinates> core::chessgame::get_available_king_moves(
    core::coordinates square)
{
  std::vector<core::coordinates> result;
  for (int steprow = -1; steprow <= 1; steprow++) {
    for (int stepcol = -1; stepcol <= 1; stepcol++) {
      if (stepcol == 0 && steprow == 0) {
        continue;
      }
      result.push_back(square);
      // only find first squares
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/false);
    }
  }
  // Handle castles
  const core::piece_t piece = this->identify_piece(square);
  if (piece == core::white_king && square == core::white_king_starting_square) {
    if (m_game_tree[m_current_position_id].white_castle_kingside
        && m_board[0][5] == core::empty_square
        && m_board[0][6] == core::empty_square)
    {
      square.file += 2;
      result.push_back(square);
      square.file -= 2;
    }
    if (m_game_tree[m_current_position_id].white_castle_queenside
        && m_board[0][3] == core::empty_square
        && m_board[0][2] == core::empty_square)
    {
      square.file -= 2;
      result.push_back(square);
      square.file += 2;
    }
  }
  if (piece == core::black_king && square == black_king_starting_square) {
    if (m_game_tree[m_current_position_id].black_castle_kingside
        && m_board[0][5] == core::empty_square
        && m_board[0][6] == core::empty_square)
    {
      square.file += 2;
      result.push_back(square);
      square.file -= 2;
    }
    if (m_game_tree[m_current_position_id].black_castle_queenside
        && m_board[0][3] == core::empty_square
        && m_board[0][2] == core::empty_square)
    {
      square.file -= 2;
      result.push_back(square);
      square.file += 2;
    }
  }
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_queen_moves(
    core::coordinates square)
{
  std::vector<core::coordinates> result;
  for (int steprow = -1; steprow <= 1; steprow++) {
    for (int stepcol = -1; stepcol <= 1; stepcol++) {
      if (stepcol == 0 && steprow == 0) {
        continue;
      }
      result.push_back(square);
      // iterate to the end of board or piece presence
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/true);
    }
  }
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_rook_moves(
    core::coordinates square)
{
  std::vector<core::coordinates> result;
  for (int steprow = -1; steprow <= 1; steprow++) {
    for (int stepcol = -1; stepcol <= 1; stepcol++) {
      if (abs(steprow) == abs(stepcol)) {
        continue;
      }
      result.push_back(square);
      // iterate to the end of the board or piece presence.
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/true);
    }
  }
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_bishop_moves(
    core::coordinates square)
{
  std::vector<core::coordinates> result;
  for (int steprow = -1; steprow <= 1; steprow++) {
    for (int stepcol = -1; stepcol <= 1; stepcol++) {
      if (steprow == 0 || stepcol == 0) {
        continue;
      }
      result.push_back(square);
      // iterate to the end of the board or piece presence
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/true);
    }
  }
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_pawn_moves(
    core::coordinates square)
{
  std::vector<coordinates> result;
  const core::piece_t piece = identify_piece(square);
  for (int steprow = -1; steprow <= 1; steprow++) {
    for (int stepcol = -1; stepcol <= 1; stepcol++) {
      // pawns don't move backwards or sidways. they can only move
      // diagonally for capture and en_passant moves. En passant
      // rules are dealt with at higher logic level. Here we
      // are just returning all possible moves regardless of
      // capture or en_passant status.
      if (steprow == 0 || (piece == core::black_pawn && steprow > 0)
          || (piece == core::white_pawn && steprow < 0))
      {
        continue;
      }
      result.push_back(square);
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/false);
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
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_knight_moves(
    core::coordinates square)
{
  std::vector<core::coordinates> result;
  for (int steprow = -2; steprow <= 2; steprow++) {
    for (int stepcol = -2; stepcol <= 2; stepcol++) {
      if (steprow == 0 || stepcol == 0 || abs(steprow) == abs(stepcol)) {
        continue;
      }
      result.push_back(square);
      // iterate only once
      draw_line(result, std::make_pair(stepcol, steprow), /*iterate=*/false);
    }
  }
  return result;
}

std::vector<core::coordinates> core::chessgame::get_available_moves(
    core::coordinates square, char piece)
{
  std::vector<core::coordinates> result;
  switch (piece) {
    case core::black_queen:
    case core::white_queen:
      result = this->get_available_queen_moves(square);
      break;
    case core::black_king:
    case core::white_king:
      result = this->get_available_king_moves(square);
      break;
    case core::white_rook:
    case core::black_rook:
      result = this->get_available_rook_moves(square);
      break;
    case core::white_bishop:
    case core::black_bishop:
      result = this->get_available_bishop_moves(square);
      break;
    case core::white_pawn:
    case core::black_pawn:
      result = get_available_pawn_moves(square);
      break;
    case core::white_knight:
    case core::black_knight:
      result = get_available_knight_moves(square);
      break;
    default:
      std::cout << "Unknown piece type provided\n";
      break;
  }

  return result;
}

core::piece_t core::chessgame::identify_piece(core::coordinates coord)
{
  const int col = coord.file - 'a';
  const int row = coord.rank - '1';
  std::cout << col << " " << row << "\n";
  return this->m_board.at(static_cast<size_t>(row))
      .at(static_cast<size_t>(col));
}

core::piece_t core::chessgame::identify_piece(int index)
{
  const int row = index / core::max_board_range;
  const int col = index % core::max_board_range;
  return this->m_board.at(static_cast<size_t>(row))
      .at(static_cast<size_t>(col));
}

void core::chessgame::show_possible_moves(core::coordinates square)
{
  const char piece = identify_piece(square);
  if (piece == core::empty_square) {
    std::cout << "There is no piece there.\n";
    return;
  }
  const std::vector<core::coordinates> choices =
      this->get_available_moves(square, piece);
  std::array<std::array<char, core::max_board_range>, core::max_board_range>
      allowed_moves_board {};
  for (int row = core::max_board_range; row >= 0; row--) {
    allowed_moves_board.at(static_cast<size_t>(row)).fill(core::empty_square);
  }
  int row = square.rank - '1';
  int col = square.file - 'a';
  allowed_moves_board.at(static_cast<size_t>(row))
      .at(static_cast<size_t>(col)) = piece;

  for (auto choices_iter : choices) {
    row = choices_iter.rank - '1';
    col = choices_iter.file - 'a';
    if (allowed_moves_board.at(static_cast<size_t>(row))
            .at(static_cast<size_t>(col))
        != core::empty_square)
    {
      continue;
    }
    allowed_moves_board.at(static_cast<size_t>(row))
        .at(static_cast<size_t>(col)) = 'X';
  }

  for (row = core::max_board_range - 1; row >= 0; row--) {
    for (col = 0; col < core::max_board_range; col++) {
      std::cout << '[';
      std::cout << allowed_moves_board.at(static_cast<size_t>(row))
                       .at(static_cast<size_t>(col));
      std::cout << ']';
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

int core::chessgame::play_move(std::pair<coordinates, coordinates> move_set)
{
  // Check that we are getting the right formatted string
  // if (move_set.length() != 4) {
  //  std::cout << "Please enter move in the format: \n";
  //  std::cout << " <startfile><startrow><endfile><endrow>\n";
  //  std::cout << "e.g. e4 -> e2e4" << "\n";
  //  return 1;
  //}
  // get source and destination strings and check for available moves
  const core::coordinates start_square = move_set.first;
  const core::coordinates end_square = move_set.second;
  const core::piece_t piece = identify_piece(start_square);
  std::cout << piece << "\n";
  bool match = false;
  const std::vector<core::coordinates> choices =
      get_available_moves(start_square, piece);
  for (const auto& choice : choices) {
    if (choice == end_square) {
      match = true;
      break;
    }
  }
  // return if the move doesn't make sense
  if (!match) {
    return 1;
  }
  // check whether there is a capture (empty squares return true)
  if (!is_valid_capture(move_set)) {
    return 1;
  }
  update_board(move_set);
  return 0;
}

bool core::chessgame::is_valid_capture(
    std::pair<core::coordinates, core::coordinates> move_set)
{
  const core::piece_t piece = identify_piece(move_set.first);
  const core::piece_t captured_piece = identify_piece(move_set.second);
  // Pawns are the only piece that changes it's move based on captures.
  if (piece == core::black_pawn || piece == core::white_pawn) {
    if (captured_piece == empty_square
        && move_set.first.file == move_set.second.file
        && abs(piece - captured_piece) > 8)
    {
      return true;
    }
    return captured_piece != core::empty_square
        && move_set.first.rank != move_set.second.rank;
  }
  if ((piece - captured_piece) > core::max_board_range) {
    return true;
  }
  return captured_piece == core::empty_square;
}

core::coordinates core::chessgame::find_king(char color)
{
  size_t temp_node_id = m_current_position_id;
  core::piece_t piece = core::empty_square;
  core::coordinates square {' ', ' '};
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
    for (size_t row = 0; row < core::max_board_range; row++) {
      for (size_t col = 0; col < core::max_board_range; col++) {
        if (m_board.at(row).at(col) == piece) {
          square.file = static_cast<char>(col + 'a');
          square.rank = static_cast<char>(row + '1');
        }
      }
    }
  }
  return square;
}

// TODO: finish implementation of checking for checked king
bool core::chessgame::king_in_check(
    std::pair<core::coordinates, core::coordinates> move_set, char color)
{
  return false;
}

// TODO: finish implementation for pinned pieces
bool core::chessgame::piece_pinned(
    std::pair<core::coordinates, core::coordinates> move_set)
{
  return false;
}

void core::chessgame::update_board(
    std::pair<core::coordinates, core::coordinates> move_set)
{
  const core::piece_t piece = identify_piece(move_set.first);
  const core::coordinates start_square = move_set.first;
  const core::coordinates end_square = move_set.second;
  const core::coordinates move_str = move_set.second;
  movenode added_movenode = m_game_tree[m_current_position_id];
  added_movenode.captured_piece = identify_piece(move_set.second);

  // Update board
  auto start_row = static_cast<size_t>(start_square.rank - '1');
  auto start_col = static_cast<size_t>(start_square.file - 'a');
  auto end_row = static_cast<size_t>(end_square.rank - '1');
  auto end_col = static_cast<size_t>(end_square.file - 'a');
  m_board.at(start_row).at(start_col) = core::empty_square;
  added_movenode.captured_piece = m_board.at(end_row).at(end_col);
  m_board.at(end_row).at(end_col) = piece;

  // Update game state
  added_movenode.pgn_move_played.push_back(static_cast<char>(piece));
  added_movenode.pgn_move_played.push_back(move_str.file);
  added_movenode.pgn_move_played.push_back(move_str.rank);
  added_movenode.move_played = move_set.second;
  added_movenode.ply++;
  added_movenode.on_move = (added_movenode.ply / 2) + 1;
  added_movenode.color_to_move = (added_movenode.ply % 2 == 0) ? 'w' : 'b';

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
    } else if (end_square.file - start_square.file == -2
               && piece == core::white_king)
    {
      m_board[0][0] = core::empty_square;
      m_board[0][3] = core::white_rook;
    } else if (end_square.file - start_square.file == 2
               && piece == core::black_king)
    {
      m_board[7][7] = core::empty_square;
      m_board[7][5] = core::black_rook;
    } else if (end_square.file - start_square.file == -2
               && piece == core::black_king)
    {
      m_board[7][0] = core::empty_square;
      m_board[7][3] = core::black_rook;
    }
  } else if (piece == core::black_rook || piece == core::white_rook) {
    if (piece == core::white_rook
        && start_square == core::coordinates('a', '1'))
    {
      added_movenode.white_castle_queenside = false;
    }
    if (piece == core::white_rook
        && start_square == core::coordinates('h', '1'))
    {
      added_movenode.white_castle_kingside = false;
    }
    if (piece == core::black_rook
        && start_square == core::coordinates('a', '8'))
    {
      added_movenode.black_castle_queenside = false;
    }
    if (piece == core::black_rook
        && start_square == core::coordinates('h', '8'))
    {
      added_movenode.black_castle_queenside = false;
    }
  }
  added_movenode.prev_move_id = m_current_position_id;

  // We shoudl find an available slot if there is one. else append.
  const size_t new_position_id = find_available_tree_id();
  if (new_position_id == 0) {
    m_game_tree.push_back(added_movenode);
    m_game_tree[m_current_position_id].sidelines.push_back(m_current_position_id
                                                           + 1);
    m_current_position_id = m_game_tree.size() - 1;
  } else {
    m_game_tree[new_position_id] = added_movenode;
    m_game_tree[m_current_position_id].sidelines.push_back(new_position_id);
    m_current_position_id = new_position_id;
  }
}

size_t core::chessgame::find_available_tree_id()
{
  size_t result = 0;
  for (size_t tree_id = 1; tree_id < m_game_tree.size(); tree_id++) {
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
  size_t position_id = 0;
  while (true) {
    pgn += m_game_tree[position_id].pgn_move_played;
    pgn += " ";
    if (m_game_tree[position_id].sidelines.empty()) {
      break;
    }
    position_id = m_game_tree[position_id].sidelines[0];
    if (m_game_tree[position_id].color_to_move == core::black_bishop) {
      pgn.push_back(static_cast<char>(m_game_tree[position_id].on_move + '1'));
      pgn += ".";
    }
  }
  return pgn;
}

std::string core::chessgame::generate_move_pgn(
    std::pair<core::coordinates, core::coordinates> move_set)
{
  std::string result;
  const core::coordinates start_square = move_set.first;
  const core::coordinates end_square = move_set.second;
  const core::piece_t piece = identify_piece(start_square);

  // once again pawns are special
  if (piece != core::white_pawn && piece != core::black_pawn) {
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
  for (auto& iter : available_moves_list) {
    if (identify_piece(iter) == piece) {
      count++;
      ambiguous_moves_list.push_back(iter);
    }
  }
  // add disambiguation symbols
  if (count > 1) {
    bool diff_row = false;
    bool diff_col = false;
    for (auto& iter : ambiguous_moves_list) {
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
