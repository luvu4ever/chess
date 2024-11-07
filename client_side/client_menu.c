#include "client_menu.h"
#include "client_auth.h"
#include "client_game.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void display_login_menu(int sock) {
    char *choices[] = {
        "Login",
        "Register",
        "Exit"
    };
    int num_choices = sizeof(choices) / sizeof(char *);
    display_menu(sock, choices, num_choices, handle_login_menu_selection);
}

void display_main_menu(int sock) {
    char *choices[] = {
        "Find Game",
        "View Profile",
        "Settings",
        "Leaderboard",
        "Help",
        "Logout"
    };
    int num_choices = sizeof(choices) / sizeof(char *);
    display_menu(sock, choices, num_choices, handle_main_menu_selection);
}

void display_menu(int sock, char *choices[], int num_choices, void (*handle_selection)(int, int)) {
    int highlight = 0;
    int choice = 0;
    int c;

    initscr(); // Start ncurses mode
    clear();
    noecho();
    cbreak();
    curs_set(0); // Hide the cursor
    keypad(stdscr, TRUE); // Enable keypad input for arrow keys

    // Create the menu loop
    while (1) {
        for (int i = 0; i < num_choices; ++i) {
            if (i == highlight) {
                attron(A_REVERSE); // Highlight the current choice
                mvprintw(i + 1, 2, choices[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 1, 2, choices[i]);
            }
        }
        c = getch();

        switch (c) {
            case KEY_UP:
            case 'w':
            case 'W':
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                highlight = (highlight + 1) % num_choices;
                break;
            case 10: // Enter key
                choice = highlight;
                handle_selection(choice, sock);
                break;
            default:
                break;
        }
    }
}

void handle_main_menu_selection(int choice, int sock) {
    switch (choice) {
        case 0:
            find_game(sock);
            break;
        case 1:
            // Implement view_profile function
            break;
        case 2:
            // Implement settings function
            break;
        case 3:
            // Implement leaderboard function
            break;
        case 4:
            // Implement help function
            break;
        case 5:
            endwin();
            exit(0);
            break;
        default:
            break;
    }
}

void handle_login_menu_selection(int choice, int sock) {
    switch (choice) {
        case 0:
            login_account(sock);
            break;
        case 1:
            register_account(sock);
            break;
        case 2:
            endwin();
            exit(0);
            break;
        default:
            break;
    }
}
