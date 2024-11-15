#ifndef GAME_HANDLER_H 
#define GAME_HANDLER_H

#include "datastructures.h"

void init_game_state(ChessBoard *board);
void handle_game_move(int player_fd, ChessBoard *board, const char *move);
void broadcast_game_state(int player1_fd, int player2_fd, ChessBoard *board);

#endif