#include "game_handler.h"
#include "chess_logic.h"
#include <stdio.h>
#include <string.h>

void init_game_state(ChessBoard *board) {
    initializeBoard(board);
    board->isWhiteTurn = true;
    board->moveCount = 0;
}

void handle_game_move(int player_fd, ChessBoard *board, const char *move) {
    // Parse move (e.g. "e2e4" to board coordinates)
    int from_x = move[0] - 'a';
    int from_y = '8' - move[1];
    int to_x = move[2] - 'a';
    int to_y = '8' - move[3];
    
    // Validate move
    if (is_valid_move(board, from_x, from_y, to_x, to_y)) {
        // Make the move
        make_move(board, from_x, from_y, to_x, to_y);
        
        // Send validation response
        char response[] = "VALID_MOVE";
        send(player_fd, response, strlen(response), 0);
        
        // Send updated board to both players
        broadcast_game_state(board->lobby->player1_fd, 
                           board->lobby->player2_fd, 
                           board);
    } else {
        char response[] = "INVALID_MOVE";
        send(player_fd, response, strlen(response), 0);
    }
}

void broadcast_game_state(int player1_fd, int player2_fd, ChessBoard *board) {
    // Send board state to both players
    sendChessBoardUpdate(player1_fd, board);
    sendChessBoardUpdate(player2_fd, board);
}