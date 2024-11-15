#include "client_game.h"
#include "client_menu.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER_SIZE 256

void client_game_loop(int sock) {
    ChessBoard board;
    bool isWhite; // Set based on server assignment
    
    // Receive initial board state
    receiveChessBoardUpdate(sock, &board);
    
    while (1) {
        // Draw current board state
        clear();
        draw_board();
        draw_pieces(&board);
        
        if ((isWhite && board.isWhiteTurn) || (!isWhite && !board.isWhiteTurn)) {
            // My turn
            handle_player_move(sock, &board);
        } else {
            // Opponent's turn
            mvprintw(20, 0, "Waiting for opponent's move...");
            refresh();
            
            // Wait for updated board after opponent's move
            receiveChessBoardUpdate(sock, &board);
        }
        
        // Check for game end conditions
        if (/* game end condition */) {
            break;
        }
    }
}

void handle_player_move(int sock, ChessBoard *board) {
    char move[5];
    mvprintw(20, 0, "Enter move (e.g. e2e4): ");
    echo();
    getstr(move);
    noecho();
    
    // Send move to server
    sendMove(sock, move);
    
    // Wait for server response
    char buffer[BUFFER_SIZE];
    recv(sock, buffer, sizeof(buffer)-1, 0);
    
    if (strcmp(buffer, "VALID_MOVE") == 0) {
        // Wait for updated board state
        receiveChessBoardUpdate(sock, board);
        
        // Redraw the board
        clear();
        draw_board();
        draw_pieces(board);
        refresh();
    } else {
        mvprintw(21, 0, "Invalid move! Try again.");
        refresh();
        napms(2000); // Show error for 2 seconds
    }
}

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

