#include "client_auth.h"
#include "client_menu.h"
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 100

// Function to send a login request to the server
void sendLoginRequest(int serverSocket, const char *username, const char *password) {
    char message[256];
    snprintf(message, sizeof(message), "LOGIN:%s:%s", username, password);
    send(serverSocket, message, strlen(message), 0);
    printf("Sent login request for username: %s\n", username);
}
// Function to receive login response from the server
void receiveLoginResponse(int serverSocket) {
    char buffer[256];
    recv(serverSocket, buffer, sizeof(buffer), 0);
    if (strstr(buffer, "LOGIN_SUCCESS") == buffer) {
        printf("Login successful!\n");
    } else {
        printf("Login failed. Please check your credentials.\n");
    }
}

void sendRegisterRequest(int serverSocket, const char *username, const char *password) {
    char message[256];
    snprintf(message, sizeof(message), "REGISTER:%s %s", username, password);
    send(serverSocket, message, strlen(message), 0);
    printf("Sent register request for username: %s\n", username);
}

void receiveRegisterResponse(int serverSocket) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    recv(serverSocket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "REGISTER_SUCCESS") == buffer) {
        printf("Registration successful. Press any key to continue to the login screen...");
    } else if (strstr(buffer, "REGISTER_FAILURE") == buffer) {
        printf("Registration failed. Username may already be taken or an error occurred. Please try again.");
    }
}

void login_account(int sock) { 
    char username[50];
    char password[50];
    char buffer[BUFFER_SIZE];

    // Clear screen and prompt user for login information
    clear();
    echo();
    mvprintw(1, 2, "Enter Username: ");
    getstr(username);
    mvprintw(2, 2, "Enter Password: ");
    getstr(password);
    noecho();

    // Send login request to the server
    sendLoginRequest(sock, username, password);

    // Wait for server response
    memset(buffer, 0, sizeof(buffer));
    read(sock, buffer, BUFFER_SIZE);

    // Check if login was successful
    if (strcmp(buffer, "LOGIN_SUCCESS") == 0) {
        mvprintw(4, 2, "Login successful. Press any key to continue to the main menu...");
        getch();
        clear();
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
    char buffer[256];

    // Clear screen and prompt user for registration information
    clear();
    echo();
    mvprintw(1, 2, "Enter Desired Username: ");
    getstr(username);
    mvprintw(2, 2, "Enter Desired Password: ");
    getstr(password);
    noecho();

    // Send registration request to the server
    sendRegisterRequest(sock, username, password);

    // Wait for server response and receive it
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);

    // Check if registration was successful
    if (strcmp(buffer, "REGISTER_SUCCESS") == 0) {
        mvprintw(4, 2, "Registration successful. Press any key to continue to the login screen...");
        getch();
        clear();
        display_login_menu(sock);
    } else {
        mvprintw(4, 2, "Registration failed: %s", buffer);
        mvprintw(5, 2, "Press any key to try again...");
        getch();
        clear();
        register_account(sock); // Retry registration
    }
}
