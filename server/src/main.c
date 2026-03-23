#include "game.h"
#include "server.h"
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SEED 1000

int main(int argc, char *argv[]) {
    srand(SEED);

    pthread_t thread_id = 0;

    int main_socket = SetupMainSocket(PORT);
    char buf[100];

    while (!IsGameStarted()) {
        struct ConnectionInfo *conn_info =
            (struct ConnectionInfo *)malloc(sizeof(struct ConnectionInfo));

        if (conn_info == NULL) {
            printf("ERROR: Failed to allocate memory for connection info\n");
            break;
        }

        conn_info->udp_their_addr_size = sizeof(conn_info->udp_their_addr);

        printf("Waiting to receive data...\n");
        int numbytes = recvfrom(main_socket, buf, sizeof(buf), 0,
                                (struct sockaddr *)&(conn_info->udp_their_addr),
                                &(conn_info->udp_their_addr_size));
        if (numbytes == -1) {
            perror("ERROR: recvfrom");
            exit(1);
        }

        char ipstr[INET_ADDRSTRLEN];

        printf(
            "INFO: got packet with length %d and first byte: %d from %s\n",
            numbytes, buf[0],
            inet_ntop(conn_info->udp_their_addr.ss_family,
                      GetInAddr((struct sockaddr *)&conn_info->udp_their_addr),
                      ipstr, sizeof(ipstr)));

        if (buf[0] != INITIAL_CONNECTION) {
            continue;
        }

        conn_info->player = AddPlayer();
        assert(pthread_create(&thread_id, NULL, RunClientThread,
                              (void *)conn_info) == 0);
    }

    printf("Starting main loop:\n");
    exit(0);
    MainLoop();

    return 0;
}
