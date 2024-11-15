#ifndef CHESS_MOVES_H
#define CHESS_MOVES_H

#include "chess_logic.h"
#include "datastructures.h"

// Function Prototypes

// Check if a given move is valid
bool is_valid_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a pawn move is valid
bool is_valid_pawn_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a knight move is valid
bool is_valid_knight_move(int from_x, int from_y, int to_x, int to_y);

// Check if a bishop move is valid
bool is_valid_bishop_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a rook move is valid
bool is_valid_rook_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a queen move is valid
bool is_valid_queen_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a king move is valid
bool is_valid_king_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a given move is castling
bool is_castling_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if an en passant move is valid
bool is_valid_en_passant(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Check if a pawn promotion is happening
bool is_pawn_promotion(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

// Promote a pawn to a new type
void promote_pawn(ChessBoard *chessBoard, int x, int y, PieceType newType);

// Check if the current player is in check
bool is_in_check(ChessBoard *chessBoard, Color color);

// Make a move on the chessboard
void make_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y);

#endif // CHESS_MOVES_H