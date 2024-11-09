#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "client_handler.h"
#include "datastructures.h"

#define PORT 8080           // Define PORT locally
#define MAX_EVENTS 10       // Define MAX_EVENTS locally
#define MAX_CLIENTS 100     // Define MAX_CLIENTS locally
#define BUFFER_SIZE 256     // Define BUFFER_SIZE locally

void set_nonblocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int server_fd, new_socket, epoll_fd;
    struct sockaddr_in address;
    struct epoll_event event, events[MAX_EVENTS];
    int opt = 1;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Set socket to listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Add server socket to epoll instance
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl failed for server socket");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Wait for events on the epoll instance
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait failed");
            close(server_fd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        // Loop through triggered events
        for (int i = 0; i < num_fds; i++) {
            if (events[i].data.fd == server_fd) {
                // Handle new client connection
                new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                if (new_socket < 0) {
                    perror("accept");
                    continue;
                }

                printf("New client connected: %d\n", new_socket);

                // Set new socket to non-blocking mode (for epoll handling)
                set_nonblocking(new_socket);

                // Create a thread to handle the client (use blocking I/O for threads)
                pthread_t threadId;
                int *clientSocketPointer = malloc(sizeof(int));
                if (clientSocketPointer == NULL) {
                    perror("malloc failed");
                    close(new_socket);
                    continue;
                }
                *clientSocketPointer = new_socket;

                // Set client socket to blocking mode for simplicity
                int flags = fcntl(new_socket, F_GETFL, 0);
                fcntl(new_socket, F_SETFL, flags & ~O_NONBLOCK);

                if (pthread_create(&threadId, NULL, handleClient, clientSocketPointer) != 0) {
                    perror("Failed to create thread");
                    close(new_socket);
                    free(clientSocketPointer);
                } else {
                    pthread_detach(threadId);  // Detach the thread for automatic cleanup
                }
            }
        }
    }

    // Close resources before exiting
    close(server_fd);
    close(epoll_fd);
    return 0;
}
