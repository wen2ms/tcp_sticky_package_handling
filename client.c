#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#include "socket.h"

int main() {
    int client_file_descriptor = create_socket();

    if (client_file_descriptor == -1) {
        return -1;
    }

    int ret = connect_to_host(client_file_descriptor, "127.0.0.1", 9999);

    if (ret == -1) {
        return -1;
    }

    int file = open("sendfile.txt", O_RDONLY);
    int file_length = 0;
    char buffer[100];

    while (1) {
        file_length = read(file, buffer, sizeof(buffer));

        if (file_length <= 0) {
            break;
        }

        send_message(client_file_descriptor, buffer, file_length);

        memset(buffer, 0, sizeof(buffer));

        usleep(300);
    }

    sleep(10);

    close_socket(client_file_descriptor);
    close(file);

    return 0;
}