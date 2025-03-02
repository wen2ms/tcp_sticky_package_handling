#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>

// Server
int set_listen(int listening_file_descriptor, unsigned short port);
int accept_connection(int listening_file_descriptor, struct sockaddr_in* addr);

// Client
int connect_to_host(int file_descriptor, const char* ip, unsigned short port);

// Shared
int create_socket();
int send_message(int file_descriptor, const char* message, int length);
int receive_message(int file_descriptor, char* message, int size);
int close_socket(int file_descriptor);

#endif  // SOCKET_H