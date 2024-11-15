#include <stdio.h>
#include <assert.h>

// Include your headers
#include "chess_logic.h"
#include "datastructures.h"
#include "handle_move.h"

// Example Test Functions
void test_initialize_board() {
    // Assuming you have a function to initialize the board
    ChessBoard board;
    initialize_board(&board);

    // Check that all the pieces are correctly set
    assert(board.squares[0][0].piece == ROOK); // Example check
    printf("test_initialize_board passed.\n");
}

void test_valid_move() {
    ChessBoard board;
    initialize_board(&board);

    // Assuming a function to check valid moves
    int is_valid = is_valid_move(&board, "e2", "e4"); // Example move
    assert(is_valid == 1); // Assuming 1 indicates valid
    printf("test_valid_move passed.\n");
}

void run_tests() {
    test_initialize_board();
    test_valid_move();
    // Add more tests here
}

int main() {
    run_tests();
    printf("All tests passed successfully.\n");
    return 0;
}
