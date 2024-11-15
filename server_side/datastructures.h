#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <stdbool.h>
#include <pthread.h>

// Maximum limits
#define MAX_PLAYERS 100
#define MAX_LOBBIES 50

// Player structure
typedef struct {
    int socket_fd;     // Socket descriptor of the player
    int game_id;       // ID of the game/lobby that the player is in (-1 if not in a game)
    char username[50]; // Username of the player
    bool is_active;    // True if the player is connected and active
} Player;

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

// Lobby structure
typedef struct {
    int player1_fd;   // Socket descriptor for player 1
    int player2_fd;   // Socket descriptor for player 2
    int lobby_id;     // Unique ID for the lobby
    bool is_active;   // True if the lobby is active
    ChessBoard gameBoard; // Chess board associated with the lobby
} Lobby;

// Declare the global variables
extern Player players[MAX_PLAYERS];
extern Lobby lobbies[MAX_LOBBIES];
extern int lobby_counter;
extern pthread_mutex_t lobby_mutex;

// Function prototypes
void initialize_lobby(Lobby *lobby, int lobby_id, int player1_fd, int player2_fd);
Lobby *create_lobby(Lobby lobbies[], int max_lobbies, int player1_fd, int player2_fd);
Player *find_player_by_username(const char *username);
void initializeBoard(ChessBoard *chessBoard);

#endif // DATASTRUCTURES_H
