#include "game.h"
#include "server.h"
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "8080"
#define SEED 1000

int main(int argc, char *argv[]) {
    srand(SEED);

    int status = 0;
    pthread_t thread_id = 0;

    int main_socket = SetupMainSocket(PORT);

    while (!IsGameStarted()) {
        struct ConnectionInfo *conn_info =
            (struct ConnectionInfo *)malloc(sizeof(struct ConnectionInfo));
        if (conn_info == NULL) {
            printf("ERROR: Failed to allocate memory for connection info\n");
            break;
        }

        conn_info->socket_fd =
            accept(main_socket, (struct sockaddr *)&(conn_info->their_addr),
                   &(conn_info->their_addr_size));
        if (status == -1) {
            fprintf(stderr, "Bind error: %d\n", errno);
            exit(1);
        }

        printf("INFO: Connection accepted\n");
        printf("INFO: Running thread with socket: %d\n", conn_info->socket_fd);
        assert(pthread_create(&thread_id, NULL, RunClientThread,
                              (void *)conn_info) == 0);
    }

    MainLoop();

    return 0;
}
