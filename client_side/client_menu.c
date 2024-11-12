#include "client_menu.h"
#include "client_auth.h"
#include "client_game.h"
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t invite_mutex = PTHREAD_MUTEX_INITIALIZER;

volatile int invite_processing = 0;

void *listen_for_invites(void *socket_desc);

void display_login_menu(int sock) {
    char *choices[] = {
        "Login",
        "Register",
        "Exit"
    };
    int num_choices = sizeof(choices) / sizeof(char *);
    handle_menu_choice(sock, choices, num_choices, handle_login_menu_selection);
}

void display_main_menu(int sock) {
    pthread_t invite_thread;
    if (pthread_create(&invite_thread, NULL, listen_for_invites, (void *)&sock) < 0) {
        perror("Could not create invite listening thread");
        return;
    }

    char *choices[] = {
        "Find Game",
        "Invite player",
        "View players available",
        "Settings",
        "Leaderboard",
        "Help",
        "Logout"
    };
    int num_choices = sizeof(choices) / sizeof(char *);
    handle_menu_choice(sock, choices, num_choices, handle_main_menu_selection);
}

void handle_menu_choice(int sock, char *choices[], int num_choices, void (*handle_selection)(int, int)) {
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
        if (invite_processing) {
            continue;
        }
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
            send_invite(sock);
            break;
        case 2:
            all_player_view(sock);
            break;
        case 3:
            // Implement settings function
                break;
        case 4:
            // Implement leaderboard function
            break;
        case 5:
            // Implement help function
            break;
        case 6:
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

#define BUFFER_SIZE 256

void *listen_for_invites(void *socket_desc) {
    int sock = *(int *)socket_desc;  // Get socket descriptor
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (read_size > 0) {
            buffer[read_size] = '\0';

            // Check if the message is an invite
            if (strncmp(buffer, "INVITE:", 7) == 0) {
                invite_processing = 1;

                // Clear and force refresh before showing the prompt
                clear();
                refresh();

                mvprintw(1, 2, "You have received an invite from another player.");
                mvprintw(2, 2, "Do you want to accept? (Y/N): ");
                refresh();

                // Ensure input is captured correctly
                echo();
                nodelay(stdscr, FALSE);  // Ensure getch() is blocking to capture input correctly

                char response;

                // Using a loop to ignore newline or invalid input
                do {
                    response = getch();
                } while (response == '\n' || response == '\r');

                noecho();

                char response_buffer[BUFFER_SIZE] = {0};

                if (response == 'y' || response == 'Y') {
                    snprintf(response_buffer, sizeof(response_buffer), "INVITE_RESPONSE ACCEPT");
                    mvprintw(3, 2, "You accepted the invite.");
                } else if (response == 'n' || response == 'N') {
                    snprintf(response_buffer, sizeof(response_buffer), "INVITE_RESPONSE REJECT");
                    mvprintw(3, 2, "You rejected the invite.");
                } else {
                    // Print out the invalid character for debugging
                    mvprintw(4, 2, "Invalid input '%c', invite ignored.", response);
                    refresh();

                    // Unlock invite processing and continue loop
                    invite_processing = 0;
                    continue;
                }

                refresh();

                // Debugging output: Verify response is being sent
                mvprintw(5, 2, "Sending response: %s", response_buffer);
                refresh();

                // Send response to server
                ssize_t bytes_sent = send(sock, response_buffer, strlen(response_buffer), 0);
                if (bytes_sent < 0) {
                    perror("Failed to send invite response");
                } else {
                    mvprintw(6, 2, "Response sent successfully.");
                }

                refresh();

                // Clear and reset invite state
                invite_processing = 0;

                // Give user a chance to see the response status
                mvprintw(7, 2, "Press any key to continue...");
                getch();
                clear();
                refresh();
            }
        } else if (read_size == 0) {
            // Server has disconnected
            printf("Server disconnected.\n");
            break;
        } else {
            perror("recv failed");
            break;
        }
    }

    return NULL;
}

