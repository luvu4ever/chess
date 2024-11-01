# Define the compiler and flags
CC = gcc
# CFLAGS = -Wall -Wextra -g
CFLAGS = -g

# Target executables
SERVER_TARGET = server
CLIENT_TARGET = client

# Source files
SERVER_SRC = server.c
CLIENT_SRC = client.c client_menu.c

# Libraries
LIBS = -lncurses

# Default target: compile both server and client
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Rule to build the server executable
$(SERVER_TARGET): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_SRC)

# Rule to build the client executable (with multiple source files and ncurses)
$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_SRC) $(LIBS)

# Clean up generated files
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
