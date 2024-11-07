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

// Function to receive the ChessBoard from the server
void receiveChessBoard(int serverSocket, ChessBoard *chessBoard) {
    ssize_t bytesReceived = recv(serverSocket, chessBoard, sizeof(ChessBoard), 0);
    if (bytesReceived == -1) {
        perror("Error receiving chess board");
    } else {
        printf("Received %zd bytes of ChessBoard from server\n", bytesReceived);
    }
}

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



// Function to request the list of available players
void requestAvailablePlayers(int serverSocket) {
    char message[256];
    snprintf(message, sizeof(message), "REQUEST_PLAYERS");
    send(serverSocket, message, strlen(message), 0);
    printf("Requested list of available players.\n");
}

// Function to receive the list of available players
void receiveAvailablePlayersList(int serverSocket) {
    char buffer[1024];
    recv(serverSocket, buffer, sizeof(buffer), 0);
    if (strstr(buffer, "PLAYERS:") == buffer) {
        char playersList[1024];
        sscanf(buffer, "PLAYERS:%[^\n]", playersList);
        printf("Available players: %s\n", playersList);
    }
}

// Function to send a challenge request to an opponent
void sendChallengeRequest(int serverSocket, const char *opponentUsername) {
    char message[256];
    snprintf(message, sizeof(message), "CHALLENGE:%s", opponentUsername);
    send(serverSocket, message, strlen(message), 0);
    printf("Sent challenge request to %s.\n", opponentUsername);
}

// Function to receive a challenge from another player
void receiveChallengeRequest(int serverSocket) {
    char buffer[256];
    recv(serverSocket, buffer, sizeof(buffer), 0);
    if (strstr(buffer, "CHALLENGE:") == buffer) {
        char challengerUsername[256];
        sscanf(buffer, "CHALLENGE:%s", challengerUsername);
        printf("Received challenge from %s\n", challengerUsername);
        
        // Ask user to accept or decline the challenge
        bool accept = false;
        printf("Do you accept the challenge from %s? (yes/no): ", challengerUsername);
        char response[10];
        scanf("%s", response);
        accept = (strcmp(response, "yes") == 0);
        
        // Send challenge response
        sendChallengeResponse(serverSocket, accept);
    }
}

// Function to send response to a challenge
void sendChallengeResponse(int serverSocket, bool acceptChallenge) {
    char message[256];
    snprintf(message, sizeof(message), "RESPONSE:%s", acceptChallenge ? "ACCEPT" : "DECLINE");
    send(serverSocket, message, strlen(message), 0);
    printf("Sent challenge response: %s\n", acceptChallenge ? "ACCEPT" : "DECLINE");
}

// Function to send a move to the server
void sendMove(int serverSocket, const char *move) {
    char message[256];
    snprintf(message, sizeof(message), "MOVE:%s", move);
    send(serverSocket, message, strlen(message), 0);
    printf("Sent move: %s\n", move);
}

void receiveChessBoardUpdate(int serverSocket, ChessBoard *chessBoard) {
    ssize_t bytesReceived = recv(serverSocket, chessBoard, sizeof(ChessBoard), 0);
    if (bytesReceived == -1) {
        perror("Error receiving chess board update");
    } else {
        printf("Received updated ChessBoard from server (%zd bytes)\n", bytesReceived);
    }
}

void receiveGameResult(int serverSocket) {
    char buffer[256];
    recv(serverSocket, buffer, sizeof(buffer), 0);
    if (strstr(buffer, "RESULT:") == buffer) {
        char result[256];
        sscanf(buffer, "RESULT:%s", result);
        printf("Game result: %s\n", result);
    }
}

// Function to send draw request to the server
void sendDrawRequest(int serverSocket) {
    char message[256];
    snprintf(message, sizeof(message), "DRAW_REQUEST");
    send(serverSocket, message, strlen(message), 0);
    printf("Sent draw request to the server.\n");
}

// Function to send resignation request to the server
void sendResignRequest(int serverSocket) {
    char message[256];
    snprintf(message, sizeof(message), "RESIGN");
    send(serverSocket, message, strlen(message), 0);
    printf("Sent resignation request to the server.\n");
}

// Function to send rematch request to the server
void sendRematchRequest(int serverSocket) {
    char message[256];
    snprintf(message, sizeof(message), "REMATCH_REQUEST");
    send(serverSocket, message, strlen(message), 0);
    printf("Sent rematch request to the server.\n");
}

// Function to receive rematch response from the server
void receiveRematchResponse(int serverSocket) {
    char buffer[256];
    recv(serverSocket, buffer, sizeof(buffer), 0);
    if (strstr(buffer, "REMATCH_ACCEPTED") == buffer) {
        printf("Rematch accepted by the opponent. Starting a new game...\n");
    } else if (strstr(buffer, "REMATCH_DECLINED") == buffer) {
        printf("Rematch declined by the opponent.\n");
    }
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    ChessBoard chessBoard;

    // Create the socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        return 1;
    }

    //TEST
    sendLoginRequest(clientSocket, "dung", "1");

    printf("Connected to server\n");
    close(clientSocket);
    return 0;
}
