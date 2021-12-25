#include "chessgame.h"

#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cmath>

Core::Chessgame::Chessgame() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    this->setBoard(fen);
}

Core::Chessgame::Chessgame(std::string fen) {
    this->setBoard(fen);
}

Core::Chessgame::~Chessgame() {}

void Core::Chessgame::setBoard(std::string fen) {
    for (int row = 0; row < 8; row++) {
        this->board[row].fill(' ');
    }

    std::cout << "Reading the standard fen string..." << std::endl;
    std::string::iterator fen_iter = fen.begin();
    int emptysquares;
    Movenode gameNode = Movenode();

    // First store the board pieces
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            if (*fen_iter == '/') {
                fen_iter++;
            }
            if (isdigit(*fen_iter)) {
                emptysquares = (*fen_iter) - 48; // Damn you ascii;
                col = col + emptysquares;
                fen_iter++;
            } else {
                board[row][col] = *fen_iter;
                fen_iter++;
            }
        }
    }

    // Handle whos move is it anyway
    fen_iter++;
    gameNode.color_to_move = *fen_iter;
    fen_iter++;

    // Handle castling rights
    fen_iter++;
    while (*fen_iter != ' ') {
        switch (*fen_iter) {
        case 'K':
            gameNode.white_castle_kingside = true;
            fen_iter++;
            break;
        case 'Q':
            gameNode.white_castle_queenside = true;
            fen_iter++;
            break;
        case 'k':
            gameNode.black_castle_kingside = true;
            fen_iter++;
            break;
        case 'q':
            gameNode.black_castle_queenside = true;
            fen_iter++;
            break;
        }
    }

    // store en_passant squares
    fen_iter++;
    if (*fen_iter != '-') {
        gameNode.en_passant = std::string(fen_iter, fen_iter + 2);
        fen_iter = fen_iter + 3;
    } else {
        gameNode.en_passant = *fen_iter;
        fen_iter = fen_iter + 2;
    }

    // store total moves and moves since last capture
    std::string::iterator start_digit = fen_iter;
    while (*fen_iter != ' ') {
        fen_iter++;
    }
    gameNode.plys_since_capture = std::stoi(std::string(start_digit, fen_iter));

    // Store current move number
    gameNode.on_move = std::stoi(std::string(fen_iter, fen.end()));

    gameTree.push_back(gameNode);
    currentPositionID = (int)gameTree.size() - 1;
}

void Core::Chessgame::show(bool flipped) {
    if (!flipped) {
        for (int row = 7; row >= 0; row--) {
            std::cout << row+1 << " ";
            for (int col = 0; col < 8; col++) {
                if (board[row][col] == ' ') {
                    std::cout << "[ ]";
                } else {
                    std::cout << '[' << this->board[row][col] << ']';
                }
            }
            std::cout << std::endl;
        }
    } else {
        for (int row = 0; row < 8; row++) {
            std::cout << row+1 << " ";
            for (int col = 7; col >= 0; col--) {
                if (board[row][col] == ' ') {
                    std::cout << "[ ]";
                } else {
                    std::cout << '[' << this->board[row][col] << ']';
                }
            }
            std::cout << std::endl;
        }
    }
    std::cout << "   ";
    if (!flipped) {
        for (int file = 0; file < 8; file++) {
            std::cout << char(file+97) << "  ";
        }
    } else {
        for (int file = 7; file >= 0; file--) {
            std::cout << char(file+97) << "  ";
        }
    }

    std::cout << std::endl;
    std::cout << gameTree[currentPositionID].color_to_move << " to move" << std::endl;
    std::cout << "on move " << gameTree[currentPositionID].on_move << std::endl;
}

std::string Core::Chessgame::getFEN() {
    std::string fen = "";
    int empty_squares = 0;
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            if (this->board[row][col] == ' ') {
                empty_squares++;
            } else {
                if (empty_squares != 0) {
                    fen += char(empty_squares + 48);
                    empty_squares = 0;
                }
                fen += this->board[row][col];
            }
        }
        if (empty_squares != 0) {
            fen += char(empty_squares + 48);
            empty_squares = 0;
        }
        fen += '/';
    }

    fen += ' ';
    fen += gameTree[currentPositionID].color_to_move;
    fen += ' ';

    if (gameTree[currentPositionID].white_castle_kingside == true) {
        fen += 'K';
    }
    if (gameTree[currentPositionID].white_castle_queenside == true) {
        fen += 'Q';
    }
    if (gameTree[currentPositionID].black_castle_kingside == true) {
        fen += 'k';
    }
    if (gameTree[currentPositionID].black_castle_queenside == true) {
        fen += 'q';
    }

    fen += ' ';
    fen += gameTree[currentPositionID].en_passant;
    fen += ' ';

    fen += std::to_string(gameTree[currentPositionID].plys_since_capture);
    fen += ' ';
    fen += std::to_string(gameTree[currentPositionID].on_move);


    return fen;
}

void Core::Chessgame::_drawLine(std::shared_ptr<std::vector<std::string> > coordinates,
                                     std::pair<int, int> direction,
                                     bool iterate) {
    int col = int(coordinates->back()[0]) - 97 + direction.first;
    int row = int(coordinates->back()[1]) - 49 + direction.second;
    if (row < 0 || row > 7 || col < 0 || col > 7) return;
    std::string new_coord;
    new_coord.push_back(char(coordinates->back()[0] + direction.first));
    new_coord.push_back(char(coordinates->back()[1] + direction.second));
    coordinates->push_back(new_coord);
    if (iterate == false) return;
    if (board[row][col] != ' ') return;
    else _drawLine(coordinates, direction, iterate);
}

std::vector<std::string> Core::Chessgame::_availableMoves(std::string square,
        char piece) {
    std::vector<std::string> result;
    std::shared_ptr<std::vector<std::string> > coordinates;
    coordinates = std::make_shared<std::vector<std::string> >();
    // Queen moves
    if (piece == 'q' || piece == 'Q') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                if (stepcol == 0 && steprow == 0) continue;
                coordinates->push_back(square);
                // iterate to the end of board or piece presence
                _drawLine(coordinates, std::make_pair(stepcol, steprow), true);
                result.insert(result.end(), coordinates->begin(), coordinates->end());
            }
        }
    }

    // King moves
    else if (piece == 'k' || piece == 'K') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                if (stepcol == 0 && steprow == 0) continue;
                coordinates->push_back(square);
                // only find first squares
                _drawLine(coordinates, std::make_pair(stepcol, steprow), false);
                result.insert(result.end(), coordinates->begin(), coordinates->end());
            }
        }
        // Handle castles
        if (piece == 'K' && square == "e1") {
            if (gameTree[currentPositionID].white_castle_kingside && board[0][5] == ' ' && board[0][6] == ' ') {
                square[0] += 2;
                result.push_back(square);
                square[0] -= 2;
            }
            if (gameTree[currentPositionID].white_castle_queenside && board[0][3] == ' ' && board[0][2] == ' ') {
                square[0] -= 2;
                result.push_back(square);
                square[0] += 2;
            }
        }
        if (piece == 'k' && square == "e8") {
            if (gameTree[currentPositionID].black_castle_kingside && board[0][5] == ' ' && board[0][6] == ' ') {
                square[0] += 2;
                result.push_back(square);
                square[0] -= 2;
            }
            if (gameTree[currentPositionID].black_castle_queenside && board[0][3] == ' ' && board[0][2] == ' ') {
                square[0] -= 2;
                result.push_back(square);
                square[0] += 2;
            }
        }
    }

    // Rook moves
    else if (piece == 'r' || piece == 'R') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                if (abs(steprow) == abs(stepcol)) continue;
                coordinates->push_back(square);
                // iterate to the end of the board or piece presence.
                _drawLine(coordinates, std::make_pair(stepcol, steprow), true);
                result.insert(result.end(), coordinates->begin(), coordinates->end());
            }
        }
    }

    // Bishop moves
    else if (piece == 'b' || piece == 'B') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                if (steprow == 0 || stepcol == 0) continue;
                coordinates->push_back(square);
                // iterate to the end of the board or piece presence
                _drawLine(coordinates, std::make_pair(stepcol, steprow), true);
                result.insert(result.end(), coordinates->begin(), coordinates->end());
            }
        }
    }

    // Pawn moves
    else if (piece == 'p' || piece == 'P') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                // pawns don't move backwards or sidways. they can only move
                // diagonally for capture and en_passant moves. En passant
                // rules are dealt with at higher logic level. Here we
                // are just returning all possible moves regardless of
                // capture or en_passant status.
                if (steprow == 0) continue;
                else if (piece == 'p' && steprow > 0) continue;
                else if (piece == 'P' && steprow < 0) continue;
                else {
                    coordinates->push_back(square);
                    _drawLine(coordinates, std::make_pair(stepcol, steprow), false);
                    result.insert(result.end(), coordinates->begin(), coordinates->end());
                }
            }
        }

        // Finally if the pawn has not moved then it can move two squares
        // forward.
        if (piece == 'p' && square[1] == '7')
            result.push_back(square.replace(1, 1, "5"));
        if (piece == 'P' && square[1] == '2')
            result.push_back(square.replace(1, 1, "4"));
    }

    // Knight moves
    else if (piece == 'n' || piece == 'N') {
        for (int steprow = -2; steprow <= 2; steprow++) {
            for (int stepcol = -2; stepcol <= 2; stepcol++) {
                if (steprow == 0 || stepcol == 0) continue;
                else if (abs(steprow) == abs(stepcol)) continue;
                else {
                    coordinates->push_back(square);
                    // iterate only once
                    _drawLine(coordinates, std::make_pair(stepcol, steprow), false);
                    result.insert(result.end(), coordinates->begin(), coordinates->end());
                }
            }
        }
    }

    return result;
}

char Core::Chessgame::identifyPiece(std::string move) {
    int col = int(move[0]) - 97;
    int row = int(move[1]) - 49;
    return this->board[row][col];
}

void Core::Chessgame::showPossibleMoves(std::string square) {
    char piece = identifyPiece(square);
    if (piece == ' ') {
        std::cout << "There is no piece there." << std::endl;
        return;
    }
    std::vector<std::string> choices = this->_availableMoves(square, piece);
    std::array<std::array<char, 8>, 8> allowedMovesBoard;
    for(int row=7; row >= 0; row--) {
        allowedMovesBoard[row].fill(' ');
    }
    allowedMovesBoard[int(square[1])-49][int(square[0])-97] = piece;

    int row;
    int col;
    for (auto choices_iter = choices.begin(); choices_iter != choices.end(); choices_iter++) {
        row = int((*choices_iter)[1]) - 49;
        col = int((*choices_iter)[0]) - 97;
        if (allowedMovesBoard[row][col] != ' ') continue;
        allowedMovesBoard[row][col] = 'X';
    }

    for (row = 7; row >= 0; row--) {
        for (col = 0; col < 8; col++) {
            if (allowedMovesBoard[row][col] == ' ') {
                std::cout << "[ ]";
            } else {
                std::cout << '[' << allowedMovesBoard[row][col] << ']';
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int Core::Chessgame::playMove(std::string move_set) {
    // Check that we are getting the right formatted string
    if (move_set.length() != 4) {
        std::cout << "Please enter move in the format: \n";
        std::cout << " <startfile><startrow><endfile><endrow>\n";
        std::cout << "e.g. e4 -> e2e4" << std::endl;
        return 1;
    }
    // get source and destination strings and check for available moves
    std::string start_square = move_set.substr(0,2);
    std::string end_square = move_set.substr(2,2);
    char piece = identifyPiece(start_square);
    bool match = false;
    std::vector<std::string> choices = _availableMoves(start_square, piece);
    for(auto iter = choices.begin(); iter != choices.end(); iter++) {
        if(*iter == end_square) {
            match = true;
            break;
        }
    }
    // return if the move doesn't make sense
    if(!match) return 1;
    // check whether there is a capture (empty squares return true)
    else if(!_validCapture(move_set)) return 1;
    else _updateBoard(move_set);
    return 0;
}

bool Core::Chessgame::_validCapture(const std::string move_set) {
    char piece = identifyPiece(move_set.substr(0, 2));
    char captured_piece = identifyPiece(move_set.substr(2, 2));
    // Pawns are the only piece that changes it's move based on captures.
    if (piece == 'p' || piece == 'P') {
        if (captured_piece == ' ' &&
                move_set.substr(0, 2)[0] == move_set.substr(2, 2)[0] &&
                (int(piece)-int(captured_piece)) > 8) return true;
        else if (captured_piece != ' ' &&
                 move_set.substr(0, 2)[1] != move_set.substr(2, 2)[1]) return true;
        else return false;
    } else if ((int(piece)-int(captured_piece)) > 8) return true;
    else if(captured_piece == ' ') return true;
    else return false;
}

std::string Core::Chessgame::_findKing(char color) {
    unsigned int temp_node_id = currentPositionID;
    char piece;
    std::string square;
    if (color == 'b') {
        piece = 'k';
        square = "e8";
    } else {
        piece = 'K';
        square = "e1";
    }

    // easiest way is to look at the moves played
    while (temp_node_id != 0) {
        if (gameTree[temp_node_id].pgn_move_played.front() == 'K' &&
                gameTree[temp_node_id].color_to_move != color) {
            square = gameTree[temp_node_id].move_played.substr(2, 2);
            break;
        }
        temp_node_id = gameTree[temp_node_id].prev_move_id;
    }

    // make sure that the king is where we think it is
    if (identifyPiece(square) != piece) {
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (board[row][col] == piece) {
                    square[0] = char(col+97);
                    square[1] = char(row+49);
                }
            }
        }
    }
    return square;
}

bool Core::Chessgame::_kingInCheck(std::string move_set, char color) {
    std::cout << move_set << " " << color << std::endl;
    return false;
}

bool Core::Chessgame::_checkPins(std::string move_set) {
    std::cout << move_set << std::endl;
    return false;
}

void Core::Chessgame::_updateBoard(std::string move_set) {
    char piece = identifyPiece(move_set.substr(0, 2));
    std::string start_square = move_set.substr(0, 2);
    std::string end_square = move_set.substr(2, 2);
    std::string move_str = move_set.substr(2, 2);
    Movenode added_movenode = gameTree[currentPositionID];
    added_movenode.captured_piece = identifyPiece(move_set.substr(2, 2));
    

    // Update board
    board[int(start_square[1]) - 49][int(start_square[0]) - 97] = ' ';
    added_movenode.captured_piece = board[int(end_square[1]) - 49][int(end_square[0]) - 97];
    board[int(end_square[1]) - 49][int(end_square[0]) - 97] = piece;

    // Update game state
    added_movenode.pgn_move_played = move_str;
    added_movenode.move_played = move_set;
    added_movenode.ply++;
    added_movenode.on_move = (added_movenode.ply / 2) + 1;
    added_movenode.color_to_move = (added_movenode.ply % 2 == 0) ? 'w' : 'b';

    if (added_movenode.captured_piece != ' ') {
        added_movenode.plys_since_capture = 0;
    } else {
        added_movenode.plys_since_capture++;
    }

    added_movenode.black_castle_queenside = gameTree[currentPositionID].black_castle_queenside;
    added_movenode.black_castle_kingside = gameTree[currentPositionID].black_castle_kingside;
    added_movenode.white_castle_queenside = gameTree[currentPositionID].white_castle_queenside;
    added_movenode.white_castle_kingside = gameTree[currentPositionID].white_castle_kingside;

    // Make sure to update en passant square
    if (piece == 'p' || piece == 'P') {
        if (start_square[0] == end_square[0] && int(start_square[1])-int(end_square[1]) > 1) {
            added_movenode.en_passant = start_square;
            // make sure en_passant square is behind the pawn
            added_movenode.en_passant[1] += (int(end_square[1]) - int(start_square[1])) / 2;
        }
    } else if (piece == 'k' || piece == 'K') {
        added_movenode.black_castle_queenside = false;
        added_movenode.black_castle_kingside = false;
        added_movenode.white_castle_queenside = false;
        added_movenode.white_castle_kingside = false;
        // handle moving the rook if this is castles
        if ((int(end_square[0]) - int(start_square[0]) == 2) && piece == 'K') {
            board[0][7] = ' ';
            board[0][5] = 'R';
        } else if ((int(end_square[0]) - int(start_square[0]) == -2) && piece == 'K') {
            board[0][0] = ' ';
            board[0][3] = 'R';
        } else if ((int(end_square[0]) - int(start_square[0]) == 2) && piece == 'k') {
            board[7][7] = ' ';
            board[7][5] = 'r';
        } else if ((int(end_square[0]) - int(start_square[0]) == -2) && piece == 'k') {
            board[7][0] = ' ';
            board[7][3] = 'r';
        }
    } else if (piece == 'r' || piece == 'R') {
        if (piece == 'R' && start_square == "a1")
            added_movenode.white_castle_queenside = false;
        if (piece == 'R' && start_square == "h1")
            added_movenode.white_castle_kingside = false;
        if (piece == 'r' && start_square == "a8")
            added_movenode.black_castle_queenside = false;
        if (piece == 'r' && start_square == "h8")
            added_movenode.black_castle_queenside = false;
    }
    added_movenode.prev_move_id = currentPositionID;

    // We shoudl find an available slot if there is one. else append.
    int newPositionID = _findAvailableTreeID();
    if (newPositionID == 0) {
        gameTree.push_back(added_movenode);
        gameTree[currentPositionID].sidelines.push_back(currentPositionID+1);
        currentPositionID = (unsigned int)gameTree.size() - 1;
    } else {
        gameTree[newPositionID] = added_movenode;
        gameTree[currentPositionID].sidelines.push_back(newPositionID);
        currentPositionID = newPositionID;
    }

    return;
}

int Core::Chessgame::_findAvailableTreeID() {
    unsigned int result = 0;
    for(unsigned int tree_id = 1; tree_id < gameTree.size(); tree_id++) {
        if (!gameTree[tree_id].linked) {
            result = tree_id;
            break;
        }
    }
    return result;
}

std::string Core::Chessgame::getPGN() {
    std::string pgn = "";
    unsigned int position_id = 0;
    while(true) {

        pgn += gameTree[position_id].pgn_move_played;
        pgn += " ";
        if (gameTree[position_id].sidelines.size() == 0) break;
        position_id = gameTree[position_id].sidelines[0];
        if (gameTree[position_id].color_to_move == 'b') {
            pgn.push_back(char(gameTree[position_id].on_move + 48));
            pgn += ".";
        }
    }
    return pgn;
}

std::string Core::Chessgame::generateMovePGN(std::string move_set) {
    std::string result = "";
    std::string start_square = move_set.substr(0, 2);
    std::string end_square = move_set.substr(2, 2);
    char piece = identifyPiece(start_square);
    
    // Start forming the string
    char pgn_piece;
    if(int(piece) > 96) {
        pgn_piece = piece - 32;
    } else {
        pgn_piece = piece;
    }

    // once again pawns are special
    if (pgn_piece != 'P') result += pgn_piece;
    
    // The idea now is to reverse engineer all squares where the 
    // piece can come from. This is to deal with cases where we
    // have ambiguity (more than one piece can land on that square)
    std::vector<std::string> available_moves; 
    std::vector<std::string> ambiguous_moves;
    if (piece == 'P') available_moves = _availableMoves(end_square, 'p');
    else if (piece == 'p') available_moves =_availableMoves(end_square, 'P');
    else available_moves = _availableMoves(end_square, piece);
    int count = 0;
    for (auto iter = available_moves.begin(); iter != available_moves.end(); iter++) {
        if (identifyPiece(*iter) == piece) {
            count++;    
            ambiguous_moves.push_back(*iter);
        }
    }
    // add disambiguation symbols
    if (count > 1) {
        bool diff_row = false;
        bool diff_col = false;
        for (auto iter = ambiguous_moves.begin(); iter != ambiguous_moves.end(); iter++) {
            if (start_square != *iter) {
                diff_row = (start_square[1] != (*iter)[1]); 
                diff_col = (start_square[0] != (*iter)[0]);
            }
        }
        if (diff_col) result += start_square[0];
        if (diff_row) result += start_square[1];
    }

    // Add captures symbol
    if (identifyPiece(end_square) != ' ') result += 'x';

    result += end_square;

    return result; 
}
