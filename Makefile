# Define the compiler and flags
CC = gcc
CFLAGS = -g -w  # -g for debugging, -w to suppress all warnings

# Target executables
SERVER_TARGET = server
CLIENT_TARGET = client

# Directories
SERVER_DIR = server_side
CLIENT_DIR = client_side

# Source files
SERVER_SRC = $(SERVER_DIR)/server.c $(SERVER_DIR)/client_handler.c $(SERVER_DIR)/auth.c
CLIENT_SRC = $(CLIENT_DIR)/client.c $(CLIENT_DIR)/client_menu.c $(CLIENT_DIR)/client_auth.c $(CLIENT_DIR)/client_game.c

# Libraries
LIBS = -lncurses -lpthread

# Default target: compile both server and client
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Rule to build the server executable
$(SERVER_TARGET): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_SRC) -lpthread

# Rule to build the client executable (with multiple source files and ncurses)
$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_SRC) $(LIBS)

# Clean up generated files
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
