#include "server.h"
#include "global_constants.h"
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int SetupMainSocket(const char *port) {
    int status = 0;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *servinfo;

    status = getaddrinfo(NULL, port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        exit(1);
    }

    int main_socket = socket(PF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    status = bind(main_socket, servinfo->ai_addr, servinfo->ai_addrlen);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    status = listen(main_socket, 20);
    if (status == -1) {
        fprintf(stderr, "Bind error: %d\n", errno);
        exit(1);
    }

    freeaddrinfo(servinfo);
    return main_socket;
}

void *RunClientThread(void *_conn_info) {
    struct ConnectionInfo *conn_info = (struct ConnectionInfo *)_conn_info;

    printf("INFO: Running thread with socket: %d\n", conn_info->socket_fd);

    const char *msg = "hi!";

    for (int i = 0; i < 5; i++) {
        sendto(conn_info->socket_fd, msg, strlen(msg), 0,
               (struct sockaddr *)(&(conn_info->their_addr)),
               conn_info->their_addr_size);
    }

    close(conn_info->socket_fd);
    free(conn_info);

    pthread_exit(NULL);
}

int n_clients;
struct ConnectionInfo clients[MAX_PLAYERS];
void BroadcastGameData(struct GameState gamestate) {
    for (int i = 0; i < n_clients; i++) {
        struct ConnectionInfo conn_info = clients[i];
        sendto(conn_info.socket_fd, (void *)&gamestate, sizeof((gamestate)), 0,
               (struct sockaddr *)(&(conn_info.their_addr)),
               conn_info.their_addr_size);
    }
}
