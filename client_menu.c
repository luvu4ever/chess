// Client Menu Implementation File (client_menu.c)
#include "client_menu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 100

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
    extern int client_socket;
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
                // display_login_menu(sock);
                break;
            }
        }
    }
}

void login_account(int sock) {
    char username[50];
    char password[50];
    char buffer[200];

    // Clear screen and prompt user for login information
    clear();
    echo();
    mvprintw(1, 2, "Enter Username: ");
    getstr(username);
    mvprintw(2, 2, "Enter Password: ");
    getstr(password);
    noecho();

    // Format the login data and send to server
    snprintf(buffer, sizeof(buffer), "LOGIN:%s:%s", username, password);
    send(sock, buffer, strlen(buffer), 0);

    // Wait for server response
    memset(buffer, 0, sizeof(buffer));
    read(sock, buffer, BUFFER_SIZE);

    // Check if login was successful
    if (strcmp(buffer, "LOGIN_SUCCESS") == 0) {
        mvprintw(4, 2, "Login successful. Press any key to continue to the main menu...");\
        getch();
        clear();
        // Call function to display the main menu here (you will need to implement display_main_menu)
        display_main_menu(sock);
    } else {
        mvprintw(4, 2, "Login failed: %s", buffer);
        mvprintw(5, 2, "Press any key to try again...");
        getch();
        clear();
        display_login_menu(sock); // Retry login
    }
}

void register_account(int sock) {
    char username[50];
    char password[50];
    char buffer[200];

    // Clear screen and prompt user for registration information
    clear();
    echo();
    mvprintw(1, 2, "Enter Username for Registration: ");
    getstr(username);
    mvprintw(2, 2, "Enter Password for Registration: ");
    getstr(password);
    noecho();

    // Format the registration data and send to server
    snprintf(buffer, sizeof(buffer), "REGISTER:%s:%s", username, password);
    send(sock, buffer, strlen(buffer), 0);

    // For now, receive server response and print it
    memset(buffer, 0, sizeof(buffer));
    read(sock, buffer, BUFFER_SIZE);

    if (strcmp(buffer, "REGISTER_SUCCESS") == 0) {
        mvprintw(4, 2, "Register successful. Press any key to continue to the main menu...");\
        getch();
        clear();
        // Call function to display the main menu here (you will need to implement display_main_menu)
        display_login_menu(sock);
    } else {
        mvprintw(4, 2, "Register failed");
        mvprintw(5, 2, "Press any key to try again...");
        getch();
        clear();
        display_login_menu(sock); // Retry login
    }
}