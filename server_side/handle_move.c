#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "chess_server.h"

// Convert chess notation (like A2) to board indices
void notation_to_indices(const char *notation, int *x, int *y) {
    *y = toupper(notation[0]) - 'A';
    *x = BOARD_SIZE - (notation[1] - '0');
}

// Send updated board state to both players
void send_board_to_players(int white_sock, int black_sock, ChessBoard *chessBoard) {
    char board_state[BUFFER_SIZE];
    snprintf(board_state, sizeof(board_state), "Board updated after move. Turn: %s", chessBoard->isWhiteTurn ? "White" : "Black");

    // In a real implementation, serialize the entire board properly
    send(white_sock, board_state, strlen(board_state), 0);
    send(black_sock, board_state, strlen(board_state), 0);
}

// Handle the move in form "White A2 A3"
void handle_move(const char *move_string, int white_sock, int black_sock, ChessBoard *chessBoard) {
    // Parse the move string
    char color_str[6], from_str[3], to_str[3];
    sscanf(move_string, "%s %s %s", color_str, from_str, to_str);

    Color player_color = (strcasecmp(color_str, "White") == 0) ? WHITE : BLACK;
    
    // Check if it's the correct player's turn
    if ((player_color == WHITE && !chessBoard->isWhiteTurn) || (player_color == BLACK && chessBoard->isWhiteTurn)) {
        char error_message[] = "Error: It's not your turn.\n";
        send((player_color == WHITE) ? white_sock : black_sock, error_message, strlen(error_message), 0);
        return;
    }

    // Convert from chess notation (e.g., A2, A3) to board indices
    int from_x, from_y, to_x, to_y;
    notation_to_indices(from_str, &from_x, &from_y);
    notation_to_indices(to_str, &to_x, &to_y);

    // Validate and make the move
    if (is_valid_move(chessBoard, from_x, from_y, to_x, to_y)) {
        make_move(chessBoard, from_x, from_y, to_x, to_y);

        // Send the updated board state to both players
        send_board_to_players(white_sock, black_sock, chessBoard);
    } else {
        // If the move is not valid, send an error message
        char invalid_message[] = "Error: Invalid move.\n";
        send((player_color == WHITE) ? white_sock : black_sock, invalid_message, strlen(invalid_message), 0);
    }
}
