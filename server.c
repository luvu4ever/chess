// Server-side code (server.c)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 20
#define BUFFER_SIZE 1024
#define MAX_EVENTS 20

void set_nonblocking(int sock);
char* handle_login(char *buffer);
char* handle_register(char *buffer);
void handle_find_game(int client_socket);
char* server_response(char *buffer, int client_socket);

static int game_socket[MAX_CLIENTS] = { -1 };

int main() {
    int server_fd, new_socket, epoll_fd;
    struct sockaddr_in address;
    struct epoll_event event, events[MAX_EVENTS];
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Setting up epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    // Add server socket to epoll instance
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_fds; i++) {
            if (events[i].data.fd == server_fd) {
                // Handle new client connection
                new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                if (new_socket < 0) {
                    perror("accept");
                    continue;
                }

                printf("New client connected: %d\n", new_socket);

                // Set new socket to non-blocking
                set_nonblocking(new_socket);

                // Add new socket to epoll instance
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                    perror("epoll_ctl failed for new socket");
                    close(new_socket);
                }
            } else {
                // Handle client input
                int client_socket = events[i].data.fd;
                char buffer[BUFFER_SIZE] = {0};
                int valread = read(client_socket, buffer, BUFFER_SIZE);

                if (valread == 0) {
                    // Connection closed by client
                    printf("Client disconnected: %d\n", client_socket);
                    close(client_socket);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                } else if (valread < 0) {
                    perror("recv");
                } else {
                    // Process client command
                    printf("Received data from %d: %s\n", client_socket, buffer);
                    char *response = server_response(buffer, client_socket);
                    if (strlen(response) > 0) {
                        send(client_socket, response, strlen(response), 0);
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}

void set_nonblocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

char* handle_login(char *buffer) {
    char userpass[100];
    strcpy(userpass, buffer + 6);
    FILE *file = fopen("user.txt", "r");
    if (file == NULL) {
        perror("Could not open user.txt");
        return "LOGIN_FAILURE";
    }

    char file_userpass[100];
    int login_success = 0;
    while (fgets(file_userpass, sizeof(file_userpass), file) != NULL) {
        file_userpass[strcspn(file_userpass, "\n")] = 0; // Remove newline character
        if (strcmp(userpass, file_userpass) == 0) {
            login_success = 1;
            break;
        }
    }
    fclose(file);
    if (login_success) {
        return "LOGIN_SUCCESS";
    } else {
        return "LOGIN_FAILURE";
    }
}

char* handle_register(char *buffer) {
    char username[50];
    char password[50];
    sscanf(buffer + 9, "%s %s", username, password);
    FILE *file = fopen("user.txt", "a+");
    if (file == NULL) {
        return "REGISTER_FAILURE";
    }
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        char existing_username[50];
        sscanf(line, "%s", existing_username);
        if (strcmp(username, existing_username) == 0) {
            fclose(file);
            return "REGISTER_FAILURE";
        }
    }
    fprintf(file, "%s:%s\n", username, password);
    fclose(file);
    return "REGISTER_SUCCESS";
}

void handle_find_game(int client_socket) {
    static int waiting_client = -1;
    static pthread_mutex_t find_game_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&find_game_mutex);
    if (waiting_client == -1) {
        waiting_client = client_socket;
        send(client_socket, "WAITING_FOR_OPPONENT", strlen("WAITING_FOR_OPPONENT"), 0);
    } else {
        // Notify both clients that a game has been found
        send(waiting_client, "GAME_FOUND", strlen("GAME_FOUND"), 0);
        send(client_socket, "GAME_FOUND", strlen("GAME_FOUND"), 0);
        waiting_client = -1;
    }
    pthread_mutex_unlock(&find_game_mutex);
}

char* server_response(char *buffer, int client_socket) {
    static char response[BUFFER_SIZE];
    memset(response, 0, BUFFER_SIZE);

    if (strncmp(buffer, "LOGIN:", 6) == 0) {
        return handle_login(buffer);
    } else if (strncmp(buffer, "REGISTER:", 9) == 0) {
        return handle_register(buffer);
    } else if (strncmp(buffer, "FIND_GAME", 9) == 0) {
        handle_find_game(client_socket);
        return ""; // No direct response for FIND_GAME; handled in `handle_find_game`
    } else {
        strcpy(response, "INVALID_COMMAND");
        return response;
    }
}
