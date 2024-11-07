#include "client_game.h"
#include "client_menu.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#define BUFFER_SIZE 100

void find_game(int sock) {
    char buffer[BUFFER_SIZE];
    int c;
    fd_set readfds;
    struct timeval tv;

    // Send find game request to server
    snprintf(buffer, sizeof(buffer), "FIND_GAME");
    send(sock, buffer, strlen(buffer), 0);

    // Wait for server to connect to another player or user to press Enter key
    clear();
    mvprintw(1, 2, "Finding a game, please wait... (Press q to cancel)");
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
            int read_size = read(sock, buffer, sizeof(buffer) - 1);
            if (read_size > 0) {
                buffer[read_size] = '\0';
                if (strcmp(buffer, "GAME_FOUND") == 0) {
                    mvprintw(2, 2, "Game found! Press any key to continue...");
                    getch();
                    clear();
                    // client_game_loop(sock);
                    break;
                }
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            c = getch();
            if (c == 'q' || c == 'Q') {
                mvprintw(2, 2, "Cancelled. Press any key to return to the main menu...");
                getch();
                clear();
                display_main_menu(sock);
                break;
            }
        }
    }
}
