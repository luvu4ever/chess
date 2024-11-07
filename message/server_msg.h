#ifndef SERVER_MSG_H
#define SERVER_MSG_H

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
void sendChessBoard(int clientSocket, ChessBoard *chessBoard);
void receiveLoginRequest(int clientSocket, char *username, char *password);
void sendLoginResponse(int clientSocket, bool success);
void receiveRegisterRequest(int clientSocket, char *username, char *password);
void sendRegisterResponse(int clientSocket, bool success);
//----------------------------------------------
void sendAvailablePlayersList(int clientSocket, const char *playersList);
void receiveChallengeRequest(int clientSocket, char *challengerUsername);
void sendChallengeToOpponent(int opponentSocket, char *challengerUsername);
void receiveChallengeResponse(int clientSocket, bool *acceptChallenge);
void receiveMove(int clientSocket, char *move, ChessBoard *chessBoard);
void sendChessBoardUpdate(int clientSocket, ChessBoard *chessBoard);
bool isMoveValid(ChessBoard *chessBoard, char *move);
void determineGameResult(ChessBoard *chessBoard, char *result);
void sendGameResult(int clientSocket, char *result);
void logGameEvent(const char *event);
void saveGameState(ChessBoard *chessBoard, char *filename);
void sendReplayData(int clientSocket, char *filename);
void receiveDrawRequest(int clientSocket);
void receiveResignRequest(int clientSocket);

#endif // SERVER_MSG_H
