#ifndef CLIENT_MSG_H
#define CLIENT_MSG_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

// Enumeration for different types of chess pieces
typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

// Enumeration for color of the chess pieces
typedef enum {
    NONE, WHITE, BLACK
} Color;

// Structure representing a chess piece
typedef struct {
    PieceType type;
    Color color;
} ChessPiece;

// Structure representing the chess board
typedef struct {
    ChessPiece board[8][8];
    ChessPiece capturedWhite[16];
    ChessPiece capturedBlack[16];
    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;
    bool isWhiteTurn;
    int moveCount;
} ChessBoard;

// Function declarations
void receiveChessBoard(int serverSocket, ChessBoard *chessBoard);
void sendLoginRequest(int serverSocket, const char *username, const char *password);
void receiveLoginResponse(int serverSocket);
void requestAvailablePlayers(int serverSocket);
void receiveAvailablePlayersList(int serverSocket);
void sendChallengeRequest(int serverSocket, const char *opponentUsername);
void receiveChallengeRequest(int serverSocket);
void sendChallengeResponse(int serverSocket, bool acceptChallenge);
void sendMove(int serverSocket, const char *move);
void receiveChessBoardUpdate(int serverSocket, ChessBoard *chessBoard);
void receiveGameResult(int serverSocket);
void sendDrawRequest(int serverSocket);
void sendResignRequest(int serverSocket);
void sendRematchRequest(int serverSocket);
void receiveRematchResponse(int serverSocket);

#endif // CLIENT_MSG_H
