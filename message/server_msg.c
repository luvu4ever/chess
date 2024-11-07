#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

typedef enum {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} PieceType;

typedef enum {
    NONE, WHITE, BLACK
} Color;

typedef struct {
    PieceType type;
    Color color;
} ChessPiece;

typedef struct {
    ChessPiece board[8][8];
    ChessPiece capturedWhite[16];
    ChessPiece capturedBlack[16];
    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;
    bool isWhiteTurn;
    int moveCount;
} ChessBoard;

void receiveLoginRequest(int clientSocket, char *username, char *password) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (strstr(buffer, "LOGIN:") == buffer) {
        sscanf(buffer, "LOGIN:%s:%s", username, password);
        printf("Received login request: Username=%s, Password=%s\n", username, password);
    }
}
// Send login response to a client
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

//-----------------------------------------------

// Send the list of available players to a client
void sendAvailablePlayersList(int clientSocket, const char *playersList) {
    char response[1024];
    snprintf(response, sizeof(response), "PLAYERS:%s", playersList);
    send(clientSocket, response, strlen(response), 0);
    printf("Sent available players list to client.\n");
}

// Receive challenge request from a client
void receiveChallengeRequest(int clientSocket, char *challengerUsername) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (strstr(buffer, "CHALLENGE:") == buffer) {
        sscanf(buffer, "CHALLENGE:%s", challengerUsername);
        printf("Received challenge request from %s\n", challengerUsername);
    }
}

// Forward the challenge to the target opponent
void sendChallengeToOpponent(int opponentSocket, char *challengerUsername) {
    char challengeMessage[256];
    snprintf(challengeMessage, sizeof(challengeMessage), "CHALLENGE:%s", challengerUsername);
    send(opponentSocket, challengeMessage, strlen(challengeMessage), 0);
    printf("Forwarded challenge to opponent: %s\n", challengerUsername);
}

// Receive challenge response from a client
void receiveChallengeResponse(int clientSocket, bool *acceptChallenge) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);

    if (strstr(buffer, "RESPONSE:ACCEPT") == buffer) {
        *acceptChallenge = true;
    } else if (strstr(buffer, "RESPONSE:DECLINE") == buffer) {
        *acceptChallenge = false;
    }
    printf("Challenge response received: %s\n", *acceptChallenge ? "ACCEPT" : "DECLINE");
}

// Receive a move from the client and validate it
void receiveMove(int clientSocket, char *move, ChessBoard *chessBoard) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (strstr(buffer, "MOVE:") == buffer) {
        sscanf(buffer, "MOVE:%s", move);
        if (isMoveValid(chessBoard, move)) {
            printf("Valid move received: %s\n", move);
            // Update the chess board with the valid move
        } else {
            printf("Invalid move received: %s\n", move);
        }
    }
}

// Send the updated chess board to the client after a move
void sendChessBoardUpdate(int clientSocket, ChessBoard *chessBoard) {
    ssize_t bytesSent = send(clientSocket, chessBoard, sizeof(ChessBoard), 0);
    if (bytesSent == -1) {
        perror("Error sending updated chess board");
    } else {
        printf("Sent updated ChessBoard to client (%zd bytes)\n", bytesSent);
    }
}

// Validate if the move is legal
bool isMoveValid(ChessBoard *chessBoard, char *move) {
    // Implement move validation logic here
    return true;  // For now, assume all moves are valid
}

// Determine the result of the game
void determineGameResult(ChessBoard *chessBoard, char *result) {
    // Logic for determining the result (e.g., checkmate, draw, stalemate)
    strcpy(result, "CHECKMATE_WHITE");
}

// Send the game result to both players
void sendGameResult(int clientSocket, char *result) {
    char message[256];
    snprintf(message, sizeof(message), "RESULT:%s", result);
    send(clientSocket, message, strlen(message), 0);
    printf("Sent game result to client: %s\n", result);
}

// Log game event (e.g., moves, messages)
void logGameEvent(const char *event) {
    FILE *logFile = fopen("game_log.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "%s\n", event);
        fclose(logFile);
        printf("Logged game event: %s\n", event);
    } else {
        perror("Error opening log file");
    }
}

// Save the current state of the chess game to a file
void saveGameState(ChessBoard *chessBoard, char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(chessBoard, sizeof(ChessBoard), 1, file);
        fclose(file);
        printf("Saved game state to file: %s\n", filename);
    } else {
        perror("Error saving game state");
    }
}

// Send replay data to a client
void sendReplayData(int clientSocket, char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        ChessBoard chessBoard;
        fread(&chessBoard, sizeof(ChessBoard), 1, file);
        sendChessBoardUpdate(clientSocket, &chessBoard);
        fclose(file);
        printf("Sent replay data from file: %s\n", filename);
    } else {
        perror("Error opening replay file");
    }
}

// Receive draw request from a client
void receiveDrawRequest(int clientSocket) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (strstr(buffer, "DRAW_REQUEST") == buffer) {
        printf("Received draw request from client\n");
        // Handle draw logic here
    }
}

// Receive resignation request from a client
void receiveResignRequest(int clientSocket) {
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    
    if (strstr(buffer, "RESIGN") == buffer) {
        printf("Received resignation from client\n");
        // Handle resignation logic here
    }
}

// Function to send the ChessBoard to a client
void sendChessBoard(int clientSocket, ChessBoard *chessBoard) {
    ssize_t bytesSent = send(clientSocket, chessBoard, sizeof(ChessBoard), 0);
    if (bytesSent == -1) {
        perror("Error sending chess board");
    } else {
        printf("Sent %zd bytes of ChessBoard to client\n", bytesSent);
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    ChessBoard chessBoard;

    // Initialize server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind and listen
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 3);

    printf("Waiting for connections...\n");
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        perror("Failed to accept client");
        return 1;
    }
    printf("Client connected!\n");

    // Send the chess board to the client
    

    close(clientSocket);
    close(serverSocket);
    return 0;
}