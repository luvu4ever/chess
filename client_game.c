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

void start_chess_game() {
    // Initialize the chess board
    ChessBoard chessBoard;
    initializeBoard(&chessBoard);

    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Black squares
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // White squares
    init_pair(5, COLOR_BLUE, COLOR_BLACK);  // Blue for white pieces on black squares
    init_pair(6, COLOR_RED, COLOR_BLACK);   // Red for black pieces on black squares
    init_pair(7, COLOR_BLUE, COLOR_WHITE);  // Blue for white pieces on white squares
    init_pair(8, COLOR_RED, COLOR_WHITE);   // Red for black pieces on white squares

    clear();
    draw_board();
    draw_pieces(&chessBoard);

    refresh();
    // input_and_move(&chessBoard);

    // Game loop
    // while (1) {
    //     // Display the updated board and pieces
    //     draw_board();
    //     draw_pieces(&chessBoard);

    //     // Handle player input and move the pieces
    //     input_and_move(&chessBoard);

    //     // Sending move data to the server
    //     // Here you should form a message with move information, e.g., "e2e4"
    //     // char moveMessage[10];
    //     // snprintf(moveMessage, sizeof(moveMessage), "MOVE %d %d %d %d", 
    //     //          /*from_x*/ 0, /*from_y*/ 1, /*to_x*/ 2, /*to_y*/ 3); // Replace with real move coordinates
    //     // send(sock, moveMessage, strlen(moveMessage), 0);
    // }

    // End ncurses mode
    endwin();
}
