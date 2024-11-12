#include "client_game.h"
#include "client_menu.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>

#define BUFFER_SIZE 256

void find_game(int sock) {
    char buffer[BUFFER_SIZE];
    int c;
    fd_set readfds;
    struct timeval tv;
    snprintf(buffer, sizeof(buffer), "FIND_GAME");
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Failed to send find game request");
        return;
    }
    clear();
    mvprintw(1, 2, "Finding a game, please wait... (Press 'q' to cancel)");
    refresh();
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 500000; // 0.5 seconds
        int activity = select(sock + 1, &readfds, NULL, NULL, &tv);
        if (activity < 0) {
            perror("select error");
            break;
        }
        if (FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (read_size > 0) {
                buffer[read_size] = '\0';
                if (strcmp(buffer, "MATCH_FOUND") == 0) {
                    mvprintw(2, 2, "Game found! Press any key to continue...");
                    refresh();
                    getch();
                    clear();
                    client_game_loop(sock);
                    break;
                } else if (strcmp(buffer, "START_GAME") == 0) {
                    client_game_loop(sock);
                    break;
                } else {
                    mvprintw(2, 2, "Unexpected server message: %s", buffer);
                    refresh();
                    getch();
                    clear();
                    break;
                }
            } else if (read_size == 0) {
                mvprintw(2, 2, "Server closed the connection. Press any key to continue...");
                refresh();
                getch();
                clear();
                break;
            } else {
                perror("recv error");
                break;
            }
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            c = getch();
            if (c == 'q' || c == 'Q') {
                mvprintw(2, 2, "Cancelled. Press any key to return to the main menu...");
                refresh();
                getch();
                clear();
                snprintf(buffer, sizeof(buffer), "CANCEL_FIND_GAME");
                send(sock, buffer, strlen(buffer), 0);
                display_main_menu(sock);
                break;
            }
        }
    }
}

void client_game_loop(int sock) {
    // Implement the game loop logic here
    // This function will handle the game state and communication with the server
    while (1) {
        // Game loop logic
        // ...
    }
}

void send_invite(int sock) {
    char targetUsername[50];
    
    // Prompt for the target player's username
    clear();
    mvprintw(1, 2, "Enter the username of the player to invite: ");
    echo();
    getnstr(targetUsername, sizeof(targetUsername) - 1);
    noecho();

    // Send the invite request to the server
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "SEND_INVITE %s", targetUsername);
    send(sock, buffer, strlen(buffer), 0);

    // Wait for a response from the server
    memset(buffer, 0, sizeof(buffer));
    int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
    printf("Received response: %s\n", buffer);
    if (read_size > 0) {
        buffer[read_size] = '\0';
        mvprintw(2, 2, "%s", buffer);
        refresh();
        getch();  // Wait for user to acknowledge the response
    }

    clear();
    refresh();
}

void all_player_view(int sock) {
    clear();
    noecho();
    char buffer[BUFFER_SIZE];
    struct timeval tv;
    fd_set readfds;
    int ret;

    // Set timeout for socket operations (5 seconds)
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    // Set socket receive timeout
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
        mvprintw(0, 2, "Failed to set socket options");
        refresh();
        napms(2000);
        return;
    }

    // Send request
    snprintf(buffer, sizeof(buffer), "VIEW_USERS");
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        mvprintw(0, 2, "Failed to send request");
        refresh();
        napms(2000);
        return;
    }

    // Clear buffer and prepare for receiving
    memset(buffer, 0, sizeof(buffer));

    // Set up select() for timeout
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    // Wait for response with timeout
    ret = select(sock + 1, &readfds, NULL, NULL, &tv);
    if (ret < 0) {
        mvprintw(0, 2, "Select error");
        refresh();
        napms(2000);
        return;
    } else if (ret == 0) {
        mvprintw(0, 2, "Operation timed out");
        refresh();
        napms(2000);
        return;
    }

    // Receive data
    int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (read_size < 0) {
        mvprintw(0, 2, "Failed to receive data");
        refresh();
        napms(2000);
        return;
    } else if (read_size == 0) {
        mvprintw(0, 2, "Server disconnected");
        refresh();
        napms(2000);
        return;
    }

    buffer[read_size] = '\0';  // Ensure null termination

    // Display header
    attron(A_BOLD);
    mvprintw(0, 2, "Available Users:");
    attroff(A_BOLD);

    // Parse and display users
    mvprintw(2, 0, "%s", buffer);

    // Display navigation instructions
    mvprintw(LINES - 2, 2, "Press any key to return to menu...");

    refresh();
    getch();
    clear();
    refresh();
}
