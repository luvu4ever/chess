#include "client_handler.h"
#include "auth.h"
#include <errno.h>   // Include errno to access error codes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256  // Define BUFFER_SIZE locally

void *handleClient(void *clientSocketPointer) {
    int clientSocket = *((int *)clientSocketPointer);
    free(clientSocketPointer);

    char username[50];
    char password[50];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        bytes_read = recv(clientSocket, buffer, sizeof(buffer), 0);

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

        if (strstr(buffer, "LOGIN:") == buffer) {
            // Handle login request
            sscanf(buffer, "LOGIN:%s %s", username, password);
            bool isValid = handle_login(username, password);
            sendLoginResponse(clientSocket, isValid);
        } else if (strstr(buffer, "REGISTER:") == buffer) {
            // Handle register request
            sscanf(buffer, "REGISTER:%s %s", username, password);
            bool isRegistered = handle_register(username, password);
            sendRegisterResponse(clientSocket, isRegistered);
        } else {
            printf("Unknown request received: %s\n", buffer);
        }
    }

    close(clientSocket);
    printf("Client connection closed: %d\n", clientSocket);
    return NULL;
}
