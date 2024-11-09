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

// Lobby structure
typedef struct {
    int player1_fd;   // Socket descriptor for player 1
    int player2_fd;   // Socket descriptor for player 2
    int lobby_id;     // Unique ID for the lobby
    bool is_active;   // True if the lobby is active
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

#endif // DATASTRUCTURES_H
