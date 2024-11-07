#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

bool handle_login(const char *username, const char *password);
bool handle_register(const char *username, const char *password);
void receiveLoginRequest(int clientSocket, char *username, char *password);
void sendLoginResponse(int clientSocket, bool success);
void receiveRegisterRequest(int clientSocket, char *username, char *password);
void sendRegisterResponse(int clientSocket, bool success);

#endif // AUTH_H
