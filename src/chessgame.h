#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <vector>
#include <array>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <memory>

namespace Core {
struct Movenode {
    bool linked = true;
    int ply = 0;
    char color_to_move = 'w';
    char captured_piece = ' ';
    std::string en_passant = "";
    bool black_castle_queenside = false;
    bool black_castle_kingside = false;
    bool white_castle_queenside = false;
    bool white_castle_kingside = false;
    int plys_since_capture = 0;
    int on_move = 0;
    std::string pgn_move_played = "";
    std::string move_played = "";
    int prev_move_id = 0;
    std::vector<int> sidelines;
};

enum Piece_t {
    WHITE_PAWN = 'P',
    WHITE_KNIGHT = 'N',
    WHITE_BISHOP = 'B',
    WHITE_ROOK = 'R',
    WHITE_QUEEN = 'Q',
    WHITE_KING = 'K',
    BLACK_PAWN = 'p',
    BLACK_KNIGHT = 'n',
    BLACK_BISHOP = 'b',
    BLACK_ROOK = 'r',
    BLACK_QUEEN = 'q',
    BLACK_KING = 'k',
    EMPTY_SQUARE = ' '
};

class Chessgame {
    private:
    std::array<std::array<char, 8>, 8> board;
    std::vector<Movenode> gameTree;
    unsigned int currentPositionID;

    void _drawLine(std::shared_ptr<std::vector<std::string> > coordinates, std::pair<int, int> direction, bool iterate);
    std::vector<std::string> _availableMoves(std::string square, char piece);
    bool _validCapture(std::string move_set);
    bool _kingInCheck(std::string move_set, char color);
    bool _checkPins(std::string move_set);
    void _updateBoard(std::string move_set);
    int _findAvailableTreeID();
    std::string _findKing(char color);

    public:
    Chessgame();
    Chessgame(std::string fen);
    ~Chessgame();

    std::string getFEN();
    std::string getPGN();
    void setBoard(std::string fen);
    std::string generateMovePGN(std::string move_set);
    int playMove(std::string move_set);
    char identifyPiece(std::string square);
    char identifyPiece(int index);

    void show(bool fliped);
    void showPossibleMoves(std::string square);

};
} // End of Core namespace

#endif
