#ifndef CHESS_GUI_H
#define CHESS_GUI_H

#include <ncurses.h>
#include <string.h>
#include <stdbool.h>

#define BOARD_SIZE 8

// Define chess pieces as an enum for simplicity
typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

// Define piece colors
typedef enum {
    NONE, WHITE, BLACK
} Color;

// Structure for individual chess pieces
typedef struct {
    PieceType type;
    Color color;
} ChessPiece;

// Structure for the chess board
typedef struct {
    ChessPiece board[8][8];    // 8x8 chess board array
    ChessPiece capturedWhite[16]; // Array for white's captured pieces (max 16)
    ChessPiece capturedBlack[16]; // Array for black's captured pieces (max 16)
    bool whiteCanCastleKingSide;  // Castling rights for white king side
    bool whiteCanCastleQueenSide; // Castling rights for white queen side
    bool blackCanCastleKingSide;  // Castling rights for black king side
    bool blackCanCastleQueenSide; // Castling rights for black queen side
    bool isWhiteTurn;             // True if it’s white's turn
    int moveCount;                // Number of moves made
} ChessBoard;

// Function declarations
void initializeBoard(ChessBoard *chessBoard);
void draw_board();
void draw_pieces(ChessBoard *chessBoard);
void move_piece(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);
void input_and_move(ChessBoard *chessBoard);
void run_chess_game();

#endif // CHESS_GUI_H
