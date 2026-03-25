#include "game.h"
#include "server.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SEED 1000

int main(int argc, char *argv[]) {
    srand(SEED);

    pthread_t thread_id = 0;

    int main_socket = SetupMainSocket(PORT);
    char buf[100];

    while (1) {
        struct ConnectionInfo *conn_info =
            (struct ConnectionInfo *)malloc(sizeof(struct ConnectionInfo));

        if (conn_info == NULL) {
            printf("ERROR: Failed to allocate memory for connection info\n");
            break;
        }

        conn_info->udp_their_addr_size = sizeof(conn_info->udp_their_addr);

        int numbytes = recvfrom(main_socket, buf, sizeof(buf), 0,
                                (struct sockaddr *)&(conn_info->udp_their_addr),
                                &(conn_info->udp_their_addr_size));
        if (numbytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                free(conn_info);
            } else {
                perror("ERROR: main UDP recv");
                free(conn_info);
                break;
            }
        } else {
            char ipstr[INET_ADDRSTRLEN];

            printf(
                "INFO: got packet with length %d and first byte: %d from %s\n",
                numbytes, buf[0],
                inet_ntop(
                    conn_info->udp_their_addr.ss_family,
                    GetInAddr((struct sockaddr *)&conn_info->udp_their_addr),
                    ipstr, sizeof(ipstr)));

            if (buf[0] == INITIAL_CONNECTION) {
                int status = SetupPlayerSockets(conn_info);
                if (status != 0) {
                    printf("ERROR: Failed to setup the player socket\n");
                    free(conn_info);
                } else {
                    struct __attribute__((packed)) {
                        char type;
                        short length;
                    } buf;
                    buf.type = INITIAL_CONNECTION;
                    buf.length = 2137;

                    sendto(conn_info->udp_socket_fd, &buf, sizeof(buf), 0,
                           (struct sockaddr *)&conn_info->udp_their_addr,
                           conn_info->udp_their_addr_size);
                    AddClient(*conn_info);
                    conn_info->player = AddPlayer();
                    assert(pthread_create(&thread_id, NULL, RunClientThread,
                                          (void *)conn_info) == 0);
                }
            } else {
                free(conn_info);
            }
        }

        TickMainLoop();
        usleep(1000 * 100);
    }

    return 0;
}
