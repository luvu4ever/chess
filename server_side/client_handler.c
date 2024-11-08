#include "client_handler.h"
#include "auth.h"
#include <errno.h>   // Include errno to access error codes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdbool.h> // Include for boolean type

#define BUFFER_SIZE 256  // Define BUFFER_SIZE locally

// Command handler struct definition
typedef struct {
    char *command;
    void (*handler)(int clientSocket, char *params);
} CommandHandler;

// Function prototypes for command handlers
void handleLoginCommand(int clientSocket, char *params);
void handleRegisterCommand(int clientSocket, char *params);

// Command handler functions
void handleLoginCommand(int clientSocket, char *params) {
    char username[50];
    char password[50];
    sscanf(params, "%s %s", username, password);
    printf("Received login request for username: %s\n", username);
    bool isValid = handle_login(username, password);
    sendLoginResponse(clientSocket, isValid);
}

void handleRegisterCommand(int clientSocket, char *params) {
    char username[50];
    char password[50];
    sscanf(params, "%s %s", username, password);
    bool isRegistered = handle_register(username, password);
    sendRegisterResponse(clientSocket, isRegistered);
}

// Main function to handle client connection
void *handleClient(void *clientSocketPointer) {
    int clientSocket = *((int *)clientSocketPointer);
    free(clientSocketPointer);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Define a list of command handlers
    CommandHandler handlers[] = {
        {"LOGIN", handleLoginCommand},
        {"REGISTER", handleRegisterCommand}
    };
    int numHandlers = sizeof(handlers) / sizeof(handlers[0]);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);  // -1 to leave space for null-terminator

        if (bytes_read < 0) {
            // Check if the read would block, if so, continue
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("recv error");
                break;
            }
        } else if (bytes_read == 0) {
            // Connection closed by client
            printf("Client disconnected: %d\n", clientSocket);
            break;
        }

        buffer[bytes_read] = '\0'; // Null-terminate the received string
        printf("Received message: %s\n", buffer);

        // Parse the command from the buffer
        char command[50];
        sscanf(buffer, "%s", command);

        // Loop through the handlers to find the matching command
        bool commandHandled = false;
        for (int i = 0; i < numHandlers; i++) {
            if (strcmp(command, handlers[i].command) == 0) {
                // Call the corresponding handler function
                char *params = buffer + strlen(command) + 1; // Get the rest of the message after the command
                handlers[i].handler(clientSocket, params);
                commandHandled = true;
                break;
            }
        }

        // If no command matches, print an unknown request message
        if (!commandHandled) {
            printf("Unknown request received: %s\n", buffer);
        }
    }

    close(clientSocket);
    printf("Client connection closed: %d\n", clientSocket);
    return NULL;
}
