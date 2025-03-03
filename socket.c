#include "socket.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int read_fixed_size(int file_descriptor, char* message, int size) {
    char* buffer = message;
    int read_count = size;

    while (read_count > 0) {
        int read_length = recv(file_descriptor, buffer, read_count, 0);

        if (read_length == -1) {
            return -1;
        } else if (read_length == 0) {
            return size - read_count;
        }

        buffer += read_length;
        read_count -= read_length;
    }

    return size;
}

int receive_message(int file_descriptor, char** message) {
    if (file_descriptor < 0 || message == NULL) {
        return -1;
    }

    int receive_data_len = 0;
    read_fixed_size(file_descriptor, (char*)&receive_data_len, 4);

    receive_data_len = ntohl(receive_data_len);
    printf("receiving data length: %d\n", receive_data_len);

    char* data = (char*)malloc(receive_data_len + 1);
    int actual_receive_data_len = read_fixed_size(file_descriptor, data, receive_data_len);

    if (actual_receive_data_len != receive_data_len) {
        printf("receiving file failed...\n");

        close(file_descriptor);
        free(data);

        return -1;
    }

    data[receive_data_len] = '\0';
    *message = data;

    return actual_receive_data_len;
}

int write_fixed_size(int file_descriptor, const char* message, int size) {
    const char* buffer = message;
    int unsend_count = size;

    while (unsend_count > 0) {
        int sent_length = send(file_descriptor, buffer, unsend_count, 0);

        if (sent_length == -1) {
            return -1;
        } else if (sent_length == 0) {
            continue;
        }

        buffer += sent_length;
        unsend_count -= sent_length;
    }

    return size;
}

int send_message(int file_descriptor, const char* message, int length) {
    if (file_descriptor < 0 || message == NULL || length <= 0) {
        return -1;
    }

    char* data_package = (char*)malloc(length + 4);

    int big_endian_length = htonl(length);

    memcpy(data_package, &big_endian_length, 4);
    memcpy(data_package + 4, message, length);


    int send_data_len = write_fixed_size(file_descriptor, data_package, length + 4);

    free(data_package);

    if (send_data_len == -1) {
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