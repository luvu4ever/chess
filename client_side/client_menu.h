#ifndef CLIENT_MENU_H
#define CLIENT_MENU_H

void display_login_menu(int sock);
void display_main_menu(int sock);
void display_menu(int sock, char *choices[], int num_choices, void (*handle_selection)(int, int));
void handle_main_menu_selection(int choice, int sock);
void handle_login_menu_selection(int choice, int sock);

#endif // CLIENT_MENU_H
