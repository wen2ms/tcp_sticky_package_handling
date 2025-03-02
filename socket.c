#include "socket.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

int create_socket() {
    int file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (file_descriptor == -1) {
        perror("socket");
        return -1;
    }

    printf("socket created successfully! file_descriptor=%d\n", file_descriptor);

    return file_descriptor;
}

int set_listen(int listening_file_descriptor, unsigned short port) {
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(listening_file_descriptor, (struct sockaddr*)&server_address, sizeof(server_address));

    if (ret == -1) {
        perror("bind");
        return -1;
    }
    printf("socket binded successfully!\n");

    ret = listen(listening_file_descriptor, 128);

    if (ret == -1) {
        perror("listen");
        return -1;
    }
    printf("setting listen successfully!\n");

    return ret;
}

int accept_connection(int listening_file_descriptor, struct sockaddr_in* addr) {
    int client_file_descriptor = -1;

    if (addr == NULL) {
        client_file_descriptor = accept(listening_file_descriptor, NULL, NULL);
    } else {
        unsigned int client_address_len = sizeof(struct sockaddr_in);

        client_file_descriptor = accept(listening_file_descriptor, (struct sockaddr*)addr, &client_address_len);
    }

    if (client_file_descriptor == -1) {
        perror("accept");
        return -1;
    }

    printf("connected to client successfully...\n");

    return client_file_descriptor;
}

int receive_message(int file_descriptor, char* message, int size) {
    int receive_data_len = recv(file_descriptor, message, size, 0);

    if (receive_data_len == 0) {
        printf("client disconnected...\n");
        close(file_descriptor);
    } else if (receive_data_len == -1) {
        perror("recv");
        close(file_descriptor);
    }

    return receive_data_len;
}

int send_message(int file_descriptor, const char* message, int length) {
    int send_data_len = send(file_descriptor, message, length, 0);

    if (send_data_len == -1) {
        perror("send");
        close(file_descriptor);
    }

    return send_data_len;
}

int connect_to_host(int file_descriptor, const char* ip, unsigned short port) {
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    inet_pton(AF_INET, ip, &server_address.sin_addr.s_addr);

    int ret = connect(file_descriptor, (struct sockaddr*)&server_address, sizeof(server_address));

    if (ret == -1) {
        perror("connect");
        return -1;
    }

    printf("connected to server successfully...\n");
    return ret;
}

int close_socket(int file_descriptor) {
    int ret = close(file_descriptor);

    if (ret == -1) {
        perror("close");
    }

    return ret;
}