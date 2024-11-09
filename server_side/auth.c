#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "auth.h"

bool handle_login(const char *username, const char *password) {
    FILE *file = fopen("user.txt", "r");
    if (file == NULL) {
        perror("Could not open user.txt");
        return false;
    }

    char line[100];
    char existing_username[50];
    char existing_password[50];
    int score, games_played, status;

    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%s %s", existing_username, existing_password);
        if (strcmp(username, existing_username) == 0 && strcmp(password, existing_password) == 0) {
            fclose(file);
            return true;
        }
    }
    
    fclose(file);
    return false;
}

bool handle_register(const char *username, const char *password) {
    FILE *file = fopen("user.txt", "a+");
    if (file == NULL) {
        perror("Could not open user.txt");
        return false;
    }

    char line[100];
    char existing_username[50];

    // Check if username already exists
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%[^:]", existing_username);
        if (strcmp(username, existing_username) == 0) {
            fclose(file);
            return false;
        }
    }

    // Write new user credentials
    fprintf(file, "%s:%s\n", username, password);
    fclose(file);
    return true;
}

void receiveLoginRequest(int clientSocket, char *username, char *password) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "LOGIN:") == buffer) {
        sscanf(buffer, "LOGIN:%s %s", username, password);
        printf("Received login request: Username=%s, Password=%s\n", username, password);
    }
}

void sendLoginResponse(int clientSocket, bool success) {
    char response[256];
    if (success) {
        snprintf(response, sizeof(response), "LOGIN_SUCCESS");
    } else {
        snprintf(response, sizeof(response), "LOGIN_FAIL");
    }
    send(clientSocket, response, strlen(response), 0);
}

void receiveRegisterRequest(int clientSocket, char *username, char *password) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "REGISTER:") == buffer) {
        sscanf(buffer, "REGISTER:%s %s", username, password);
        printf("Received register request: Username=%s, Password=%s\n", username, password);
    }
}

void sendRegisterResponse(int clientSocket, bool success) {
    char response[256];
    if (success) {
        snprintf(response, sizeof(response), "REGISTER_SUCCESS");
    } else {
        snprintf(response, sizeof(response), "REGISTER_FAILURE");
    }
    send(clientSocket, response, strlen(response), 0);
}
