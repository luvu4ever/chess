#include "datastructures.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_PLAYERS 100
#define MAX_LOBBIES 50
#define BOARD_SIZE 8

Player players[MAX_PLAYERS];
Lobby lobbies[MAX_LOBBIES];          // Array of lobbies
int lobby_counter = 0;               // Counter to assign unique lobby IDs
pthread_mutex_t lobby_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for protecting shared data
// int waiting_player_socket = -1;      // No player waiting initially

// Initialize a lobby
void initialize_lobby(Lobby *lobby, int lobby_id, int player1_fd, int player2_fd) {
    lobby->player1_fd = player1_fd;
    lobby->player2_fd = player2_fd;
    lobby->lobby_id = lobby_id;
    lobby->is_active = true;
}


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


// Create a new lobby
Lobby *create_lobby(Lobby lobbies[], int max_lobbies, int player1_fd, int player2_fd) {
    for (int i = 0; i < max_lobbies; i++) {
        if (!lobbies[i].is_active) {
            initialize_lobby(&lobbies[i], lobby_counter++, player1_fd, player2_fd);
            return &lobbies[i];
        }
    }
    return NULL;  // No available lobby slots
}

Player *find_player_by_username(const char *username) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].is_active && strcmp(players[i].username, username) == 0) {
            return &players[i];
        }
    }
    return NULL; // No player found with that username
}

// Initialize the chess board
void initializeBoard(ChessBoard *chessBoard) {
    // Initialize all squares to EMPTY
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            chessBoard->board[row][col].type = EMPTY;
            chessBoard->board[row][col].color = NONE;
        }
    }

    // Set up white pieces
    for (int col = 0; col < BOARD_SIZE; ++col) {
        chessBoard->board[1][col].type = PAWN;
        chessBoard->board[1][col].color = WHITE;
    }
    chessBoard->board[0][0].type = ROOK; chessBoard->board[0][0].color = WHITE;
    chessBoard->board[0][7].type = ROOK; chessBoard->board[0][7].color = WHITE;
    chessBoard->board[0][1].type = KNIGHT; chessBoard->board[0][1].color = WHITE;
    chessBoard->board[0][6].type = KNIGHT; chessBoard->board[0][6].color = WHITE;
    chessBoard->board[0][2].type = BISHOP; chessBoard->board[0][2].color = WHITE;
    chessBoard->board[0][5].type = BISHOP; chessBoard->board[0][5].color = WHITE;
    chessBoard->board[0][3].type = QUEEN; chessBoard->board[0][3].color = WHITE;
    chessBoard->board[0][4].type = KING; chessBoard->board[0][4].color = WHITE;

    // Set up black pieces
    for (int col = 0; col < BOARD_SIZE; ++col) {
        chessBoard->board[6][col].type = PAWN;
        chessBoard->board[6][col].color = BLACK;
    }
    chessBoard->board[7][0].type = ROOK; chessBoard->board[7][0].color = BLACK;
    chessBoard->board[7][7].type = ROOK; chessBoard->board[7][7].color = BLACK;
    chessBoard->board[7][1].type = KNIGHT; chessBoard->board[7][1].color = BLACK;
    chessBoard->board[7][6].type = KNIGHT; chessBoard->board[7][6].color = BLACK;
    chessBoard->board[7][2].type = BISHOP; chessBoard->board[7][2].color = BLACK;
    chessBoard->board[7][5].type = BISHOP; chessBoard->board[7][5].color = BLACK;
    chessBoard->board[7][3].type = QUEEN; chessBoard->board[7][3].color = BLACK;
    chessBoard->board[7][4].type = KING; chessBoard->board[7][4].color = BLACK;

    // Initialize captured pieces array to empty
    for (int i = 0; i < 16; ++i) {
        chessBoard->capturedWhite[i].type = EMPTY;
        chessBoard->capturedWhite[i].color = NONE;
        chessBoard->capturedBlack[i].type = EMPTY;
        chessBoard->capturedBlack[i].color = NONE;
    }

    // Set castling rights to true initially
    chessBoard->whiteCanCastleKingSide = true;
    chessBoard->whiteCanCastleQueenSide = true;
    chessBoard->blackCanCastleKingSide = true;
    chessBoard->blackCanCastleQueenSide = true;

    // White starts
    chessBoard->isWhiteTurn = true;
    chessBoard->moveCount = 0;
}
