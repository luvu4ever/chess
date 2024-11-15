#include "chess_logic.h"
#define BOARD_SIZE 8

// Check if a given move is valid
bool is_valid_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    // Check boundaries
    if (from_x < 0 || from_x >= BOARD_SIZE || from_y < 0 || from_y >= BOARD_SIZE ||
        to_x < 0 || to_x >= BOARD_SIZE || to_y < 0 || to_y >= BOARD_SIZE) {
        return false;
    }

    // Get the piece to move
    ChessPiece piece = chessBoard->board[from_x][from_y];
    if (piece.type == EMPTY) {
        return false; // No piece at the from position
    }

    // Ensure the correct player is moving
    if ((piece.color == WHITE && !chessBoard->isWhiteTurn) || 
        (piece.color == BLACK && chessBoard->isWhiteTurn)) {
        return false;
    }

    // Prevent moving to a square occupied by own piece
    ChessPiece targetPiece = chessBoard->board[to_x][to_y];
    if (targetPiece.color == piece.color) {
        return false;
    }

    // Check piece-specific movement rules
    switch (piece.type) {
        case PAWN:
            return is_valid_pawn_move(chessBoard, from_x, from_y, to_x, to_y);
        case KNIGHT:
            return is_valid_knight_move(from_x, from_y, to_x, to_y);
        case BISHOP:
            return is_valid_bishop_move(chessBoard, from_x, from_y, to_x, to_y);
        case ROOK:
            return is_valid_rook_move(chessBoard, from_x, from_y, to_x, to_y);
        case QUEEN:
            return is_valid_queen_move(chessBoard, from_x, from_y, to_x, to_y);
        case KING:
            return is_valid_king_move(chessBoard, from_x, from_y, to_x, to_y);
        default:
            return false;
    }
}

// Check if a pawn move is valid
bool is_valid_pawn_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    ChessPiece pawn = chessBoard->board[from_x][from_y];
    int direction = (pawn.color == WHITE) ? -1 : 1; // White moves up, Black moves down

    // Standard move
    if (to_y == from_y && to_x == from_x + direction && chessBoard->board[to_x][to_y].type == EMPTY) {
        return true;
    }

    // Initial double move
    if ((pawn.color == WHITE && from_x == 6) || (pawn.color == BLACK && from_x == 1)) {
        if (to_y == from_y && to_x == from_x + 2 * direction &&
            chessBoard->board[from_x + direction][to_y].type == EMPTY &&
            chessBoard->board[to_x][to_y].type == EMPTY) {
            return true;
        }
    }

    // Capture move
    if (to_x == from_x + direction && (to_y == from_y + 1 || to_y == from_y - 1) &&
        chessBoard->board[to_x][to_y].type != EMPTY &&
        chessBoard->board[to_x][to_y].color != pawn.color) {
        return true;
    }

    // En passant (to be implemented as an additional feature if desired)

    return false;
}

// Check if a knight move is valid
bool is_valid_knight_move(int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

// Check if a bishop move is valid
bool is_valid_bishop_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    if (dx == dy) {
        // Check path for obstructions
        int x_dir = (to_x - from_x) / dx;
        int y_dir = (to_y - from_y) / dy;
        for (int step = 1; step < dx; ++step) {
            if (chessBoard->board[from_x + step * x_dir][from_y + step * y_dir].type != EMPTY) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Check if a rook move is valid
bool is_valid_rook_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    if (from_x == to_x) {
        // Moving along the row
        int y_dir = (to_y > from_y) ? 1 : -1;
        for (int y = from_y + y_dir; y != to_y; y += y_dir) {
            if (chessBoard->board[from_x][y].type != EMPTY) {
                return false;
            }
        }
        return true;
    } else if (from_y == to_y) {
        // Moving along the column
        int x_dir = (to_x > from_x) ? 1 : -1;
        for (int x = from_x + x_dir; x != to_x; x += x_dir) {
            if (chessBoard->board[x][from_y].type != EMPTY) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Check if a queen move is valid (combination of rook and bishop)
bool is_valid_queen_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    return is_valid_rook_move(chessBoard, from_x, from_y, to_x, to_y) ||
           is_valid_bishop_move(chessBoard, from_x, from_y, to_x, to_y);
}

// Check if a king move is valid (excluding castling)
bool is_valid_king_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    return (dx <= 1 && dy <= 1);
}

// Check if a given move is castling
bool is_castling_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    ChessPiece king = chessBoard->board[from_x][from_y];

    // Ensure it's the king moving and attempting to castle
    if (king.type != KING || from_y != to_y || abs(to_x - from_x) != 2) {
        return false;
    }

    // King-side castling
    if (to_x == from_x + 2) {
        if (king.color == WHITE && chessBoard->whiteCanCastleKingSide) {
            // Ensure no pieces between king and rook, and no checks
            if (chessBoard->board[7][5].type == EMPTY && chessBoard->board[7][6].type == EMPTY &&
                !is_in_check(chessBoard, WHITE) &&
                !is_in_check_after_move(chessBoard, 7, 4, 7, 5, WHITE)) {
                return true;
            }
        } else if (king.color == BLACK && chessBoard->blackCanCastleKingSide) {
            if (chessBoard->board[0][5].type == EMPTY && chessBoard->board[0][6].type == EMPTY &&
                !is_in_check(chessBoard, BLACK) &&
                !is_in_check_after_move(chessBoard, 0, 4, 0, 5, BLACK)) {
                return true;
            }
        }
    }

    // Queen-side castling
    if (to_x == from_x - 2) {
        if (king.color == WHITE && chessBoard->whiteCanCastleQueenSide) {
            if (chessBoard->board[7][1].type == EMPTY && chessBoard->board[7][2].type == EMPTY &&
                chessBoard->board[7][3].type == EMPTY &&
                !is_in_check(chessBoard, WHITE) &&
                !is_in_check_after_move(chessBoard, 7, 4, 7, 3, WHITE)) {
                return true;
            }
        } else if (king.color == BLACK && chessBoard->blackCanCastleQueenSide) {
            if (chessBoard->board[0][1].type == EMPTY && chessBoard->board[0][2].type == EMPTY &&
                chessBoard->board[0][3].type == EMPTY &&
                !is_in_check(chessBoard, BLACK) &&
                !is_in_check_after_move(chessBoard, 0, 4, 0, 3, BLACK)) {
                return true;
            }
        }
    }

    return false;
}

bool is_pawn_promotion(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    ChessPiece pawn = chessBoard->board[from_x][from_y];
    if (pawn.type == PAWN) {
        if ((pawn.color == WHITE && to_x == 0) || (pawn.color == BLACK && to_x == 7)) {
            return true;
        }
    }
    return false;
}

void promote_pawn(ChessBoard *chessBoard, int x, int y, PieceType newType) {
    ChessPiece *pawn = &chessBoard->board[x][y];
    if (pawn->type == PAWN) {
        // Promote to the given type (QUEEN, ROOK, BISHOP, KNIGHT)
        pawn->type = newType;
    }
}

bool is_in_check(ChessBoard *chessBoard, Color color) {
    // Find the king's position
    int king_x = -1, king_y = -1;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            ChessPiece piece = chessBoard->board[i][j];
            if (piece.type == KING && piece.color == color) {
                king_x = i;
                king_y = j;
                break;
            }
        }
    }

    if (king_x == -1 || king_y == -1) {
        return false; // King not found (this should not happen in a valid game state)
    }

    // Check if any opponent piece can attack the king
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            ChessPiece attacker = chessBoard->board[i][j];
            if (attacker.color != color && attacker.color != NONE) {
                if (is_valid_move(chessBoard, i, j, king_x, king_y)) {
                    return true;
                }
            }
        }
    }

    return false;
}


void make_move(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    if (is_valid_move(chessBoard, from_x, from_y, to_x, to_y)) {
        ChessPiece *movedPiece = &chessBoard->board[from_x][from_y];
        ChessPiece targetPiece = chessBoard->board[to_x][to_y];

        // Update captured pieces
        if (targetPiece.type != EMPTY) {
            if (targetPiece.color == WHITE) {
                chessBoard->capturedWhite[chessBoard->moveCount] = targetPiece;
            } else {
                chessBoard->capturedBlack[chessBoard->moveCount] = targetPiece;
            }
        }

        // Handle castling move
        if (is_castling_move(chessBoard, from_x, from_y, to_x, to_y)) {
            if (to_x > from_x) {
                // King-side castling
                chessBoard->board[from_x][from_y + 1] = chessBoard->board[from_x][7];
                chessBoard->board[from_x][7].type = EMPTY;
            } else {
                // Queen-side castling
                chessBoard->board[from_x][from_y - 1] = chessBoard->board[from_x][0];
                chessBoard->board[from_x][0].type = EMPTY;
            }
        }

        // Move the piece
        chessBoard->board[to_x][to_y] = *movedPiece;
        movedPiece->type = EMPTY;
        movedPiece->color = NONE;

        // Update castling rights if rook or king moved
        if (movedPiece->type == KING) {
            if (movedPiece->color == WHITE) {
                chessBoard->whiteCanCastleKingSide = false;
                chessBoard->whiteCanCastleQueenSide = false;
            } else {
                chessBoard->blackCanCastleKingSide = false;
                chessBoard->blackCanCastleQueenSide = false;
            }
        } else if (movedPiece->type == ROOK) {
            if (from_x == 7 && from_y == 0) chessBoard->whiteCanCastleQueenSide = false;
            if (from_x == 7 && from_y == 7) chessBoard->whiteCanCastleKingSide = false;
            if (from_x == 0 && from_y == 0) chessBoard->blackCanCastleQueenSide = false;
            if (from_x == 0 && from_y == 7) chessBoard->blackCanCastleKingSide = false;
        }

        // Update turn
        chessBoard->isWhiteTurn = !chessBoard->isWhiteTurn;
        chessBoard->moveCount++;
    }
}
