// Client Menu Header File (client_menu.h)
#ifndef CLIENT_MENU_H
#define CLIENT_MENU_H

#include <ncurses.h>

void display_login_menu(int sock);
void display_main_menu(int sock);
void display_menu(int sock, char *choices[], int num_choices, void (*handle_selection)(int, int));
void handle_login_menu_selection(int choice, int sock);
void handle_main_menu_selection(int choice, int sock);
void find_game(int sock);
void login_account(int sock);
void register_account(int sock);

#endif // CLIENT_MENU_H