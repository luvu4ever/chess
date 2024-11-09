#include "datastructures.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_PLAYERS 100
#define MAX_LOBBIES 50

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