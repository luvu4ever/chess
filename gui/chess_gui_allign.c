#include <ncurses.h>
#include "engine/board.h"
#include "engine/game.h"
#include "engine/pieces.h"
#include <string.h>

#define BOARD_SIZE 8

void draw_board() {
    int start_x = 4, start_y = 2;
    int square_width = 6, square_height = 3;

    // Draw column labels
    for (int j = 0; j < BOARD_SIZE; j++) {
        mvprintw(start_y - 1, start_x + j * square_width + square_width / 2, "%c", 'A' + j);
    }

    // Draw row labels
    for (int i = 0; i < BOARD_SIZE; i++) {
        mvprintw(start_y + i * square_height + square_height / 2, start_x - 2, "%d", BOARD_SIZE - i);
    }

    // Draw the board squares
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Calculate the top-left corner of each square
            int x = start_x + j * square_width;
            int y = start_y + i * square_height;

            // Set the color pair based on the square position
            if ((i + j) % 2 == 0) {
                attron(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
            }

            // Draw the square
            for (int dy = 0; dy < square_height; dy++) {
                mvhline(y + dy, x, ' ', square_width);
            }

            // Turn off the color pair
            if ((i + j) % 2 == 0) {
                attroff(COLOR_PAIR(1));
            } else {
                attroff(COLOR_PAIR(2));
            }
        }
    }
}

void draw_pieces(Game *g) {
    int start_x = 4, start_y = 2;
    int square_width = 6, square_height = 3;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = g->b[i][j];
            if (piece != EMPTY_SQUARE) {
                // Calculate the position to draw the piece
                int x = start_x + j * square_width + square_width / 2;
                int y = start_y + i * square_height + square_height / 2;

                // Set the color based on the piece type and square color
                if ((i + j) % 2 == 0) { // Black square
                    if (get_piece_owner(piece) == WHITE) {
                        attron(COLOR_PAIR(5)); // Blue piece on black square
                    } else {
                        attron(COLOR_PAIR(6)); // Red piece on black square
                    }
                } else { // White square
                    if (get_piece_owner(piece) == WHITE) {
                        attron(COLOR_PAIR(7)); // Blue piece on white square
                    } else {
                        attron(COLOR_PAIR(8)); // Red piece on white square
                    }
                }

                // Print the piece in uppercase
                char piece_char;
                switch (get_piece_type(piece)) {
                    case PAWN: piece_char = 'P'; break;
                    case ROOK: piece_char = 'R'; break;
                    case KNIGHT: piece_char = 'K'; break;
                    case BISHOP: piece_char = 'B'; break;
                    case QUEEN: piece_char = 'Q'; break;
                    case KING: piece_char = 'T'; break;
                    default: piece_char = '?'; break;
                }

                mvprintw(y, x, "%c", piece_char);

                // Turn off the color pair
                if ((i + j) % 2 == 0) {
                    if (get_piece_owner(piece) == WHITE) {
                        attroff(COLOR_PAIR(5));
                    } else {
                        attroff(COLOR_PAIR(6));
                    }
                } else {
                    if (get_piece_owner(piece) == WHITE) {
                        attroff(COLOR_PAIR(7));
                    } else {
                        attroff(COLOR_PAIR(8));
                    }
                }
            }
        }
    }
}

void input_move(Game *g) {
    char input[10];
    int from_x, from_y, to_x, to_y;

    mvprintw(20, 60, "Enter your move (e.g., e2 e4) or type 'escape' to quit:");
    echo();
    mvgetnstr(21, 60, input, 10);
    noecho();

    if (strcmp(input, "escape") == 0) {
        endwin();
        exit(0);
    }

    // Parse the input
    if (sscanf(input, "%c%d %c%d", &from_x, &from_y, &to_x, &to_y) == 4) {
        from_x = from_x - 'a';
        from_y = BOARD_SIZE - from_y;
        to_x = to_x - 'a';
        to_y = BOARD_SIZE - to_y;

        // Make the move using the game structure
        int move_result = make_move(g, from_x, from_y, to_x, to_y);
        if (move_result == INVALID_MOVE) {
            mvprintw(22, 60, "Invalid move. Please try again.");
        }
    } else {
        mvprintw(22, 60, "Invalid input. Please try again.");
    }

    // Redraw the board and pieces
    clear();
    draw_board();
    draw_pieces(g);
    refresh();
}

int main() {
    Game game = make_game();

    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Black squares
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // White squares
    init_pair(5, COLOR_BLUE, COLOR_BLACK);  // White pieces on black squares
    init_pair(6, COLOR_RED, COLOR_BLACK);   // Black pieces on black squares
    init_pair(7, COLOR_BLUE, COLOR_WHITE);  // White pieces on white squares
    init_pair(8, COLOR_RED, COLOR_WHITE);   // Black pieces on white squares

    clear();
    draw_board();
    draw_pieces(&game);

    refresh();

    while (1) {
        input_move(&game);
    }

    getch();
    endwin();

    return 0;
}
