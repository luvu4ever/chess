// void handle_find_game(int client_socket) {
//     static int waiting_client = -1;
//     static pthread_mutex_t find_game_mutex = PTHREAD_MUTEX_INITIALIZER;

//     pthread_mutex_lock(&find_game_mutex);
//     if (waiting_client == -1) {
//         waiting_client = client_socket;
//         send(client_socket, "WAITING_FOR_OPPONENT", strlen("WAITING_FOR_OPPONENT"), 0);
//     } else {
//         // Notify both clients that a game has been found
//         send(waiting_client, "GAME_FOUND", strlen("GAME_FOUND"), 0);
//         send(client_socket, "GAME_FOUND", strlen("GAME_FOUND"), 0);
//         waiting_client = -1;
//     }
//     pthread_mutex_unlock(&find_game_mutex);
// }

// char* server_response(char *buffer, int client_socket) {
//     static char response[BUFFER_SIZE];
//     memset(response, 0, BUFFER_SIZE);

//     if (strncmp(buffer, "LOGIN:", 6) == 0) {
//         return handle_login(buffer);
//     } else if (strncmp(buffer, "REGISTER:", 9) == 0) {
//         return handle_register(buffer);
//     } else if (strncmp(buffer, "FIND_GAME", 9) == 0) {
//         handle_find_game(client_socket);
//         return ""; // No direct response for FIND_GAME; handled in `handle_find_game`
//     } else {
//         strcpy(response, "INVALID_COMMAND");
//         return response;
//     }
// }