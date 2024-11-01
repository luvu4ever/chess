#include <ncurses.h>
#include <string.h>
#include <stdbool.h>

#define BOARD_SIZE 8

// Define chess pieces as an enum for simplicity
typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

// Define piece colors
typedef enum {
    NONE, WHITE, BLACK
} Color;

// Structure for individual chess pieces
typedef struct {
    PieceType type;
    Color color;
} ChessPiece;

// Structure for the chess board
typedef struct {
    ChessPiece board[8][8];    // 8x8 chess board array
    ChessPiece capturedWhite[16]; // Array for white's captured pieces (max 16)
    ChessPiece capturedBlack[16]; // Array for black's captured pieces (max 16)
    bool whiteCanCastleKingSide;  // Castling rights for white king side
    bool whiteCanCastleQueenSide; // Castling rights for white queen side
    bool blackCanCastleKingSide;  // Castling rights for black king side
    bool blackCanCastleQueenSide; // Castling rights for black queen side
    bool isWhiteTurn;             // True if it’s white's turn
    int moveCount;                // Number of moves made
} ChessBoard;

// Initialize the chess board
void initializeBoard(ChessBoard *chessBoard) {
    // Initialize all squares to EMPTY
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            chessBoard->board[row][col].type = EMPTY;
            chessBoard->board[row][col].color = NONE;
        }
    }

    // Set up white pieces
    for (int col = 0; col < BOARD_SIZE; ++col) {
        chessBoard->board[1][col].type = PAWN;
        chessBoard->board[1][col].color = WHITE;
    }
    chessBoard->board[0][0].type = ROOK; chessBoard->board[0][0].color = WHITE;
    chessBoard->board[0][7].type = ROOK; chessBoard->board[0][7].color = WHITE;
    chessBoard->board[0][1].type = KNIGHT; chessBoard->board[0][1].color = WHITE;
    chessBoard->board[0][6].type = KNIGHT; chessBoard->board[0][6].color = WHITE;
    chessBoard->board[0][2].type = BISHOP; chessBoard->board[0][2].color = WHITE;
    chessBoard->board[0][5].type = BISHOP; chessBoard->board[0][5].color = WHITE;
    chessBoard->board[0][3].type = QUEEN; chessBoard->board[0][3].color = WHITE;
    chessBoard->board[0][4].type = KING; chessBoard->board[0][4].color = WHITE;

    // Set up black pieces
    for (int col = 0; col < BOARD_SIZE; ++col) {
        chessBoard->board[6][col].type = PAWN;
        chessBoard->board[6][col].color = BLACK;
    }
    chessBoard->board[7][0].type = ROOK; chessBoard->board[7][0].color = BLACK;
    chessBoard->board[7][7].type = ROOK; chessBoard->board[7][7].color = BLACK;
    chessBoard->board[7][1].type = KNIGHT; chessBoard->board[7][1].color = BLACK;
    chessBoard->board[7][6].type = KNIGHT; chessBoard->board[7][6].color = BLACK;
    chessBoard->board[7][2].type = BISHOP; chessBoard->board[7][2].color = BLACK;
    chessBoard->board[7][5].type = BISHOP; chessBoard->board[7][5].color = BLACK;
    chessBoard->board[7][3].type = QUEEN; chessBoard->board[7][3].color = BLACK;
    chessBoard->board[7][4].type = KING; chessBoard->board[7][4].color = BLACK;

    // Initialize captured pieces array to empty
    for (int i = 0; i < 16; ++i) {
        chessBoard->capturedWhite[i].type = EMPTY;
        chessBoard->capturedWhite[i].color = NONE;
        chessBoard->capturedBlack[i].type = EMPTY;
        chessBoard->capturedBlack[i].color = NONE;
    }

    // Set castling rights to true initially
    chessBoard->whiteCanCastleKingSide = true;
    chessBoard->whiteCanCastleQueenSide = true;
    chessBoard->blackCanCastleKingSide = true;
    chessBoard->blackCanCastleQueenSide = true;

    // White starts
    chessBoard->isWhiteTurn = true;
    chessBoard->moveCount = 0;
}

// Function to draw the board using the ChessBoard struct
void draw_board() {
    int start_x = 2, start_y = 1;
    int square_width = 6, square_height = 3;

    // Draw column labels
    for (int j = 0; j < BOARD_SIZE; j++) {
        mvprintw(start_y - 1, start_x + j * square_width + square_width / 2, "%c", 'A' + j);
    }

    // Draw row labels
    for (int i = 0; i < BOARD_SIZE; i++) {
        mvprintw(start_y + i * square_height + square_height / 2, start_x - 2, "%d", BOARD_SIZE - i);
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = start_x + j * square_width;
            int y = start_y + i * square_height;

            if ((i + j) % 2 == 0) {
                attron(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
            }

            for (int dy = 0; dy < square_height; dy++) {
                mvhline(y + dy, x, ' ', square_width);
            }

            if ((i + j) % 2 == 0) {
                attroff(COLOR_PAIR(1));
            } else {
                attroff(COLOR_PAIR(2));
            }
        }
    }
}

// Function to draw pieces using the ChessBoard struct
void draw_pieces(ChessBoard *chessBoard) {
    int start_x = 2, start_y = 1;
    int square_width = 6, square_height = 3;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            ChessPiece piece = chessBoard->board[i][j];
            if (piece.type != EMPTY) {
                int x = start_x + j * square_width + square_width / 2;
                int y = start_y + i * square_height + square_height / 2;
                char pieceChar;

                // Determine the character representation of the piece
                switch (piece.type) {
                    case PAWN:   pieceChar = 'P'; break;
                    case KNIGHT: pieceChar = 'K'; break;
                    case BISHOP: pieceChar = 'B'; break;
                    case ROOK:   pieceChar = 'R'; break;
                    case QUEEN:  pieceChar = 'Q'; break;
                    case KING:   pieceChar = 'T'; break; // Using 'T' to represent the King
                    default:     pieceChar = ' ';
                }

                // Set color based on piece color and background square color
                if ((i + j) % 2 == 0) {
                    if (piece.color == WHITE) {
                        attron(COLOR_PAIR(5)); // Blue for white pieces on black square
                    } else if (piece.color == BLACK) {
                        attron(COLOR_PAIR(6)); // Red for black pieces on black square
                    }
                } else {
                    if (piece.color == WHITE) {
                        attron(COLOR_PAIR(7)); // Blue for white pieces on white square
                    } else if (piece.color == BLACK) {
                        attron(COLOR_PAIR(8)); // Red for black pieces on white square
                    }
                }

                mvprintw(y, x, "%c", pieceChar);

                // Turn off the color
                if ((i + j) % 2 == 0) {
                    if (piece.color == WHITE) {
                        attroff(COLOR_PAIR(5));
                    } else if (piece.color == BLACK) {
                        attroff(COLOR_PAIR(6));
                    }
                } else {
                    if (piece.color == WHITE) {
                        attroff(COLOR_PAIR(7));
                    } else if (piece.color == BLACK) {
                        attroff(COLOR_PAIR(8));
                    }
                }
            }
        }
    }
}

void move_piece(ChessBoard *chessBoard, int from_x, int from_y, int to_x, int to_y) {
    // Move the piece in the board
    chessBoard->board[to_y][to_x] = chessBoard->board[from_y][from_x];
    chessBoard->board[from_y][from_x].type = EMPTY;
    chessBoard->board[from_y][from_x].color = NONE;

    // Redraw the board and pieces
    clear();
    draw_board();
    draw_pieces(chessBoard);
    refresh();
}

void input_and_move(ChessBoard *chessBoard) {
    char input[10];
    int input_start_x = 60;  // Set input starting x-coordinate to be on the right side of the board
    int input_start_y = 2;
    while (1) {
        mvprintw(input_start_y, input_start_x, "Enter move (e.g., A2 A3) or type 'escape' to quit:");
        mvprintw(input_start_y + 1, input_start_x, "");
        echo();
        getstr(input);
        noecho();

        if (strcmp(input, "escape") == 0) {
            break;
        }

        if (strlen(input) == 5 && input[2] == ' ') {
            int from_x = input[0] - 'A';
            int from_y = BOARD_SIZE - (input[1] - '0');
            int to_x = input[3] - 'A';
            int to_y = BOARD_SIZE - (input[4] - '0');

            if (from_x >= 0 && from_x < BOARD_SIZE && from_y >= 0 && from_y < BOARD_SIZE &&
                to_x >= 0 && to_x < BOARD_SIZE && to_y >= 0 && to_y < BOARD_SIZE) {
                move_piece(chessBoard, from_x, from_y, to_x, to_y);
            } else {
                mvprintw(input_start_y + 3, input_start_x, "Invalid move. Please try again.");
            }
        } else {
            mvprintw(input_start_y + 3, input_start_x, "Invalid input format. Please use format 'A2 A3'.");
        }
    }
}

void run_chess_game() {
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
    input_and_move(&chessBoard);

    endwin();
}