#include "chess_gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Assuming socket is handled elsewhere and passed here for communication
extern int sock;

void start_chess_game();

int main() {
    start_chess_game();
    return 0;
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