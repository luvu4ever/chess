#include "client_handler.h"
#include "auth.h"
#include "datastructures.h"
#include <errno.h>   // Include errno to access error codes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h> // Include for boolean type

#define BUFFER_SIZE 256  // Define BUFFER_SIZE locally
#define MAX_PLAYERS 100  // Define MAX_PLAYERS locally
#define MAX_LOBBIES 50   // Define MAX_LOBBIES locally

// Command handler struct definition
typedef struct {
    char *command;
    void (*handler)(int clientSocket, char *params);
} CommandHandler;

extern Player players[MAX_PLAYERS]; // Global array of players
extern Lobby lobbies[MAX_LOBBIES];  // Global array of lobbies
extern int lobby_counter;           // Counter for assigning lobby IDs
extern pthread_mutex_t lobby_mutex; // Mutex for thread safety
int waiting_player_socket = -1;     // Global variable to track waiting player
char USER_FILE[] = "user.txt";     // File to store user data

// Command handler functions
void handleLoginCommand(int clientSocket, char *params) {
    char username[50];
    char password[50];
    sscanf(params, "%s %s", username, password);
    printf("Received login request for username: %s\n", username);

    // Authenticate the user
    bool isValid = handle_login(username, password);
    sendLoginResponse(clientSocket, isValid);

    if (isValid) {
        // Update the user's status in the file to "active" (1)
        printf("Player %s logged in and marked as active.\n", username);

        // Add the player to the players array
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!players[i].is_active) {
                strcpy(players[i].username, username);
                players[i].socket_fd = clientSocket;
                players[i].is_active = true;
                printf("Player %s added to the players array.\n", username);
                break;
            }
        }
    }
}

void handleRegisterCommand(int clientSocket, char *params) {
    char username[50];
    char password[50];
    sscanf(params, "%s %s", username, password);
    bool isRegistered = handle_register(username, password);
    sendRegisterResponse(clientSocket, isRegistered);
}

void handleFindgameCommand(int clientSocket, char *params) {
    pthread_mutex_lock(&lobby_mutex);  // Lock mutex to ensure thread-safe access to shared data
    if (waiting_player_socket == -1) {
        // No player is currently waiting, set the current client as the waiting player
        waiting_player_socket = clientSocket;
        // char waitMessage[] = "WAIT: You are now waiting for an opponent to join...\n";
        // send(clientSocket, waitMessage, strlen(waitMessage), 0);0
    } else {
        // There is already a player waiting, match the current client with the waiting player
        int player1_fd = waiting_player_socket;
        int player2_fd = clientSocket;
        // Create a new lobby
        Lobby *new_lobby = create_lobby(lobbies, MAX_LOBBIES, player1_fd, player2_fd);
        if (new_lobby != NULL) {
            printf("New lobby created with ID: %d for players %d and %d\n", new_lobby->lobby_id, player1_fd, player2_fd);
            waiting_player_socket = -1;
            char matchMessage[] = "MATCH_FOUND";
            send(player1_fd, matchMessage, strlen(matchMessage), 0);
            send(player2_fd, matchMessage, strlen(matchMessage), 0);
            start_game_for_clients(player1_fd, player2_fd);
        } else {
            char errorMessage[] = "ERROR: Unable to create a lobby at the moment. Please try again later.\n";
            send(clientSocket, errorMessage, strlen(errorMessage), 0);
        }
    }
    pthread_mutex_unlock(&lobby_mutex);  // Unlock mutex after shared data modification
}

void handleSendInviteCommand(int clientSocket, char *params) {
    char invitedUsername[50];
    sscanf(params, "%s", invitedUsername);
    printf("Received invite request from client %d to invite user: %s\n", clientSocket, invitedUsername);
    // Find the invited player in the players array
    int invitedSocket = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].is_active && strcmp(players[i].username, invitedUsername) == 0) {
            invitedSocket = players[i].socket_fd;
            break;
        }
    }
    if (invitedSocket != -1) {
        // Send an invite message to the invited player
        char inviteMessage[BUFFER_SIZE];
        snprintf(inviteMessage, sizeof(inviteMessage), "INVITE: %d", clientSocket);
        send(invitedSocket, inviteMessage, strlen(inviteMessage), 0);
        printf("Invite sent to user: %s (socket %d)\n", invitedUsername, invitedSocket);
    } else {
        // If the invited player is not found or not active, notify the client
        char errorMessage[] = "ERROR: User not found or not active.\n";
        send(clientSocket, errorMessage, strlen(errorMessage), 0);
        printf("Invite failed: User %s not found or not active.\n", invitedUsername);
    }
}

void handleInviteResponse(int clientSocket, char *params) {
    char response[10];
    int invitingSocket;
    sscanf(params, "%s %d", response, &invitingSocket);
    printf("Received invite response from client %d: %s\n", clientSocket, response);
    if (strcmp(response, "ACCEPT") == 0) {
        pthread_mutex_lock(&lobby_mutex);  // Lock mutex to ensure thread-safe access to shared data
        Lobby *new_lobby = create_lobby(lobbies, MAX_LOBBIES, invitingSocket, clientSocket);
        if (new_lobby != NULL) {
            printf("New lobby created with ID: %d for players %d and %d\n", new_lobby->lobby_id, invitingSocket, clientSocket);
            char matchMessage[] = "MATCH_FOUND";
            send(invitingSocket, matchMessage, strlen(matchMessage), 0);
            send(clientSocket, matchMessage, strlen(matchMessage), 0);
            start_game_for_clients(invitingSocket, clientSocket);
        } else {
            char errorMessage[] = "ERROR: Unable to create a lobby at the moment. Please try again later.\n";
            send(clientSocket, errorMessage, strlen(errorMessage), 0);
            send(invitingSocket, errorMessage, strlen(errorMessage), 0);
        }
        pthread_mutex_unlock(&lobby_mutex);  // Unlock mutex after shared data modification
    } else if (strcmp(response, "DECLINE") == 0) {
        char declineMessage[] = "INVITE_DECLINED";
        send(invitingSocket, declineMessage, strlen(declineMessage), 0);
        printf("Invite from client %d declined by client %d\n", invitingSocket, clientSocket);
    } else {
        char errorMessage[] = "ERROR: Invalid response.\n";
        send(clientSocket, errorMessage, strlen(errorMessage), 0);
        printf("Invalid invite response received from client %d: %s\n", clientSocket, response);
    }
}

void handleMoveCommand(int clientSocket, char *params) {
    // Find player's active game
    Lobby *lobby = find_player_lobby(clientSocket);
    if (!lobby) {
        char error[] = "ERROR: Not in active game";
        send(clientSocket, error, strlen(error), 0);
        return;
    }
    
    // Handle the move
    handle_game_move(clientSocket, &lobby->gameBoard, params);
}

void start_game_for_clients(int player1_fd, int player2_fd) {
    // Initialize game state
    ChessBoard gameBoard;
    init_game_state(&gameBoard);

    // Store game state in lobby
    for (int i = 0; i < MAX_LOBBIES; i++) {
        if (lobbies[i].player1_fd == player1_fd && 
            lobbies[i].player2_fd == player2_fd) {
            lobbies[i].gameBoard = gameBoard;
            break;
        }
    }

    // Notify players and send initial board state
    char startMessage[] = "START_GAME";
    send(player1_fd, startMessage, strlen(startMessage), 0);
    send(player2_fd, startMessage, strlen(startMessage), 0);
    
    broadcast_game_state(player1_fd, player2_fd, &gameBoard);
}

// Main function to handle client connection
void *handleClient(void *clientSocketPointer) {
    int clientSocket = *((int *)clientSocketPointer);
    free(clientSocketPointer);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Define a list of command handlers
    CommandHandler handlers[] = {
        {"LOGIN", handleLoginCommand},
        {"REGISTER", handleRegisterCommand},
        {"FIND_GAME", handleFindgameCommand},
        {"SEND_INVITE", handleSendInviteCommand},
        {"INVITE_RESPONSE", handleInviteResponse},
        {"MOVE", handleMoveCommand},
    };
    int numHandlers = sizeof(handlers) / sizeof(handlers[0]);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);  // -1 to leave space for null-terminator

        if (bytes_read < 0) {
            // Check if the read would block, if so, continue
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("recv error");
                break;
            }
        } else if (bytes_read == 0) {
            // Connection closed by client
            printf("Client disconnected: %d\n", clientSocket);
            break;
        }
        
        buffer[bytes_read] = '\0'; // Null-terminate the received string
        printf("Received message: %s\n", buffer);

        // Parse the command from the buffer
        char command[50];
        sscanf(buffer, "%s", command);

        // Loop through the handlers to find the matching command
        bool commandHandled = false;
        for (int i = 0; i < numHandlers; i++) {
            if (strcmp(command, handlers[i].command) == 0) {
                // Call the corresponding handler function
                char *params = buffer + strlen(command) + 1; // Get the rest of the message after the command
                handlers[i].handler(clientSocket, params);
                commandHandled = true;
                break;
            }
        }

        // If no command matches, print an unknown request message
        if (!commandHandled) {
            printf("Unknown request received: %s\n", buffer);
        }
    }

    close(clientSocket);
    printf("Client connection closed: %d\n", clientSocket);
    return NULL;
}
