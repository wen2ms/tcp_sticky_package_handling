#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "socket.h"

struct SocketInfo {
    struct sockaddr_in client_address;
    int client_file_descriptor;
};

struct SocketInfo socket_infos[512];

void* worker(void* arg) {
    printf("worker id=%p...\n", pthread_self());

    struct SocketInfo* p_info = (struct SocketInfo*)arg;

    char ip[32];
    printf("Client IP: %s, Port: %d\n", inet_ntop(AF_INET, &p_info->client_address.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(p_info->client_address.sin_port));

    while (1) {
        char* buffer;

        int receive_data_len = receive_message(p_info->client_file_descriptor, &buffer);

        printf("receiving data: %d ......\n", receive_data_len);

        if (receive_data_len > 0) {
            printf("%s\n\n\n\n", buffer);

            free(buffer);
        } else {
            break;
        }

        sleep(1);
    }
    
    p_info->client_file_descriptor = -1;

    return NULL;
}

int main() {
    int server_file_descriptor = create_socket();

    if (server_file_descriptor == -1) {
        return -1;
    }

    int ret = set_listen(server_file_descriptor, 9999);

    if (ret == -1) {
        return -1;
    }

    int socket_infos_max = sizeof(socket_infos) / sizeof(socket_infos[0]);

    for (int i = 0; i <socket_infos_max; ++i) {
        bzero(&socket_infos[i], sizeof(socket_infos[i]));

        socket_infos[i].client_file_descriptor = -1;
    }

    while (1) {
        struct SocketInfo* p_info;

        for (int i = 0; i < socket_infos_max; ++i) {
            if (socket_infos[i].client_file_descriptor == -1) {
                p_info = &socket_infos[i];
                break;
            }
        }

        p_info->client_file_descriptor = accept_connection(server_file_descriptor, &p_info->client_address);

        pthread_t tid;

        pthread_create(&tid, NULL, worker, p_info);
        
        pthread_detach(tid);
    }

    close_socket(server_file_descriptor);

    return 0;
}