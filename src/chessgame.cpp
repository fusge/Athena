#include "chessgame.h"

#include <iterator>
#include <iostream>
#include <cstdlib>
#include <cmath>

chesspeer::chessgame::chessgame() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    this->setBoard(fen);
}

chesspeer::chessgame::chessgame(std::string fen) {
    this->setBoard(fen);
}

chesspeer::chessgame::~chessgame(){};

void chesspeer::chessgame::setBoard(std::string fen) {
    for (int row = 0; row < 8; row++) {
        this->board[row].fill(' ');
    }

    std::cout << "Reading the standard fen string..." << std::endl;
    std::string::iterator fen_iter = fen.begin();
    int emptysquares;
    std::shared_ptr<chesspeer::Movenode> gameNode;
    gameNode = std::make_shared<chesspeer::Movenode>();

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
            }
            else {
                board[row][col] = *fen_iter;
                fen_iter++;
            }
        }
    }

    // Handle whos move is it anyway
    fen_iter++;
    gameNode->color_to_move = *fen_iter;
    fen_iter++;

    // Handle castling rights
    fen_iter++;
    while (*fen_iter != ' ') {
        switch (*fen_iter) {
        case 'K': gameNode->white_castle_kingside = true;
        case 'Q': gameNode->white_castle_queenside = true;
        case 'k': gameNode->black_castle_kingside = true;
        case 'q': gameNode->black_castle_queenside = true;
        default: fen_iter++;
        }
    }

    // store en_passant squares
    fen_iter++;
    if (*fen_iter != '-') {
        gameNode->en_passant = std::string(fen_iter, fen_iter + 2);
        fen_iter = fen_iter + 3;
    }
    else {
        gameNode->en_passant = *fen_iter;
        fen_iter = fen_iter + 2;
    }

    // store total moves and moves since last capture
    std::string::iterator start_digit = fen_iter;
    while (*fen_iter != ' ') {
        fen_iter++;
    }
    gameNode->plys_since_capture = std::stoi(std::string(start_digit, fen_iter));

    // Store current move number
    gameNode->on_move = std::stoi(std::string(fen_iter, fen.end()));

    gameTree = gameNode;
    currentPosition = gameNode;
}

void chesspeer::chessgame::show(bool flipped) {
    if (!flipped){
        for (int row = 7; row >= 0; row--) {
            std::cout << row+1 << " ";
            for (int col = 0; col < 8; col++) {
                if (board[row][col] == ' ') {
                    std::cout << "[ ]";
                }
                else {
                    std::cout << '[' << this->board[row][col] << ']';
                }
            }
            std::cout << std::endl;
        }
    }
    else{
        for (int row = 0; row < 8; row++){
            std::cout << row+1 << " ";
            for (int col = 7; col >= 0; col--){
                if (board[row][col] == ' ') {
                    std::cout << "[ ]";
                }
                else {
                    std::cout << '[' << this->board[row][col] << ']';
                }
            }
            std::cout << std::endl;
        }
    }
    std::cout << "   ";
    if (!flipped){
        for (int file = 0; file < 8; file++){
            std::cout << char(file+97) << "  ";
        }
    }
    else {
        for (int file = 7; file >= 0; file--){
            std::cout << char(file+97) << "  ";
        }
    }
    
    std::cout << std::endl;
    std::cout << currentPosition->color_to_move << " to move" << std::endl;
    std::cout << "on move " << currentPosition->on_move << std::endl;
}

std::string chesspeer::chessgame::getFen() {
    std::string fen = "";
    int empty_squares = 0;
    for (int row = 7; row >= 0; row--){
        for (int col = 0; col < 8; col++) {
            if (this->board[row][col] == ' ') {
                empty_squares++;
            }
            else {
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
    fen += this->gameTree->color_to_move;
    fen += ' ';

    if (this->gameTree->white_castle_kingside == true) {
        fen += 'K';
    }
    if (this->gameTree->white_castle_queenside == true) {
        fen += 'Q';
    }
    if (this->gameTree->black_castle_kingside == true) {
        fen += 'k';
    }
    if (this->gameTree->black_castle_queenside == true) {
        fen += 'q';
    }

    fen += ' ';
    fen += this->gameTree->en_passant;
    fen += ' ';

    fen += std::to_string(this->gameTree->plys_since_capture);
    fen += ' ';
    fen += std::to_string(this->gameTree->on_move);
    

    return fen;
}

void chesspeer::chessgame::_drawLine(std::shared_ptr<std::list<std::string> > coordinates,
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
    if (this->board[row+direction.second][col+direction.first] != ' ') return;
    else this->_drawLine(coordinates, direction, iterate);
}

std::list<std::string> chesspeer::chessgame::_availableMoves(std::string square,
                                                             char piece) {

    std::list<std::string> result;
    std::shared_ptr<std::list<std::string> > coordinates;
    coordinates = std::make_shared<std::list<std::string> >();
    // Queen moves
    if (piece == 'q' || piece == 'Q') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                if (stepcol == 0 && steprow == 0) continue;
                coordinates->push_back(square);
                // iterate to the end of board or piece presence
                _drawLine(coordinates, std::make_pair(stepcol, steprow), true);
                result.splice(result.end(), *coordinates);
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
                // Castling is so complex it will be dealt at higher level
                // using the state information from chessgame class.
                result.splice(result.end(), *coordinates);
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
                result.splice(result.end(), *coordinates);
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
                result.splice(result.end(), *coordinates);
            }
        }
    }

    // Pawn moves
    else if (piece == 'p' || piece == 'P') {
        for (int steprow = -1; steprow <= 1; steprow++) {
            for (int stepcol = -1; stepcol <= 1; stepcol++) {
                // pawns don't move backwards or sidways. they can only move 
                // diagonally for capture and en_passant moves. those complex
                // rules are dealt with at higher logic level. We only want
                // the possible squares to move to.
                if (steprow == 0) continue;
                else if (piece == 'p' && steprow > 0) continue;
                else if (piece == 'P' && steprow < 0) continue;
                else {
                    coordinates->push_back(square);
                    _drawLine(coordinates, std::make_pair(stepcol, steprow), false);
                    result.splice(result.end(), *coordinates);
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
                    result.splice(result.end(), *coordinates);
                }
            }
        }
    }

    return result;
}

char chesspeer::chessgame::identifyPiece(std::string move){
    int col = int(move[0]) - 97;
    int row = int(move[1]) - 49;
    return this->board[row][col];
}

void chesspeer::chessgame::showPossibleMoves(std::string square){
    char piece = identifyPiece(square);
    if (piece == ' '){
        std::cout << "There is no piece there." << std::endl;
        return;
    }
    std::list<std::string> choices = this->_availableMoves(square, piece);
    std::array<std::array<char, 8>, 8> allowedMovesBoard; 
    for(int row=7; row >= 0; row--){
        allowedMovesBoard[row].fill(' ');
    }
    allowedMovesBoard[int(square[1])-49][int(square[0])-97] = piece;

    int row;
    int col;
    for (auto choices_iter = choices.begin(); choices_iter != choices.end(); choices_iter++){
        row = int((*choices_iter)[1]) - 49;
        col = int((*choices_iter)[0]) - 97;
        if (allowedMovesBoard[row][col] != ' ') continue;
        allowedMovesBoard[row][col] = 'X';
    }
    
    for (row = 7; row >= 0; row--) {
        for (col = 0; col < 8; col++) {
            if (allowedMovesBoard[row][col] == ' ') {
                std::cout << "[ ]";
            }
            else {
                std::cout << '[' << allowedMovesBoard[row][col] << ']';
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int chesspeer::chessgame::playMove(std::string move_set){
    // Check that we are getting the right formatted string
    if (move_set.length() != 4){
        std::cout << "Please enter move in the format: \n";
        std::cout << " <startfile><startrow><endfile><endrow>\n";
        std::cout << "e.g. e4 -> e2e4" << std::endl;
        return 1;
    }
    
    // get source and destination strings and check for available moves
    std::string start_square = move_set.substr(0,2);
    std::string end_square = move_set.substr(2,2);
    bool match = false;
    std::list<std::string> choices = _availableMoves(start_square, identifyPiece(start_square));
    for(auto iter = choices.begin(); iter != choices.end(); iter++){
        if(*iter == end_square){
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

bool chesspeer::chessgame::_validCapture(const std::string move_set){
    char piece = identifyPiece(move_set.substr(0, 2));
    char captured_piece = identifyPiece(move_set.substr(2, 2));
    if ((int(piece)-int(captured_piece)) > 8) return true;
    else if(captured_piece == ' ') return true;
    else return false;
}

std::string chesspeer::chessgame::_findKing(char color){
    std::shared_ptr<chesspeer::Movenode> temp_node = currentPosition;
    char piece;
    std::string square;
    if (color == 'b'){
        piece = 'k';
        square = "e8";
    }
    else{
        piece = 'K';
        square = "e1";
    }

    while (temp_node->prevmove != nullptr){
        if (temp_node->pgn_move_played.front() == 'K' && temp_node->color_to_move != color){
            square = temp_node->move_played.substr(2, 2);
            break;
        }
        temp_node = temp_node->prevmove;
    }
    return square;
}

bool chesspeer::chessgame::_kingInCheck(std::string move_set, char color){
    return false;
}

bool chesspeer::chessgame::_checkPins(std::string move_set){
    return false;
}

void chesspeer::chessgame::_updateBoard(std::string move_set){
    char piece = identifyPiece(move_set.substr(0, 2));
    std::string start_square = move_set.substr(0, 2);
    std::string end_square = move_set.substr(2, 2);
    std::string move_str = move_set.substr(2, 2);
    if(int(piece) > 96)
        move_str.insert(0, std::string(1, piece-32));
    else
        move_str.insert(0, std::string(1, piece));
    
    std::shared_ptr<Movenode> added_movenode = std::make_shared<Movenode>();
    //*added_movenode = *currentPosition;
    
    // Update board
    board[int(start_square[1])-49][int(start_square[0])-97] = ' ';
    added_movenode->captured_piece = board[int(end_square[1])-49][int(end_square[0])-97];
    board[int(end_square[1])-49][int(end_square[0])-97] = piece;
    
    // Update game state
    added_movenode->pgn_move_played = move_str;
    added_movenode->move_played = move_set;
    added_movenode->ply++;
    added_movenode->captured_piece = identifyPiece(move_set.substr(2, 2));
    
    if (currentPosition->color_to_move == 'w'){
        added_movenode->color_to_move = 'b';
    }
    else{
        added_movenode->color_to_move = 'w';
        added_movenode->on_move++;
    }
    
    if (added_movenode->captured_piece != ' '){
        added_movenode->plys_since_capture = 0;
    }
    else{
        added_movenode->plys_since_capture++;
    }
    
    added_movenode->prevmove = currentPosition;
    
    currentPosition->sidelines.push_back(added_movenode);
    currentPosition = added_movenode;

    return; 
}
