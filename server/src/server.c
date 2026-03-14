#include "server.h"
#include "global_constants.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <fcntl.h>
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
    hints.ai_socktype = SOCK_DGRAM;
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

int SetupPlayerSockets(struct ConnectionInfo *conn_info) {
    int status = connect(conn_info->socket_fd,
                         (const struct sockaddr *)&(conn_info->their_addr),
                         conn_info->their_addr_size);
    if (status != 0) {
        perror("Failed to connect the player socket: ");
        close(conn_info->socket_fd);
        free(conn_info);

        return status;
    }

    printf("INFO: Running thread with socket: %d\n", conn_info->socket_fd);
    // Establish player name here, before the socket becomes non-blocking

    fcntl(conn_info->socket_fd, F_SETFL, O_NONBLOCK);

    conn_info->udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *servinfo;

    status = getaddrinfo(
        NULL, "9090", &hints,
        &servinfo); // Use port 0 to get the next free port on calling bind

    if (status != 0) {
        fprintf(stderr, "getaddrinfo() Error: %s\n", gai_strerror(status));
        return status;
    }

    status =
        bind(conn_info->udp_socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);

    freeaddrinfo(servinfo);
    if (status == -1 && errno != EADDRINUSE) {
        fprintf(stderr, "Error: assignin the next free socket failed");
        return status;
    }

    // Get info about the player UDP socket
    status = getsockname(conn_info->udp_socket_fd,
                         (struct sockaddr *)&conn_info->udp_their_addr,
                         &conn_info->udp_their_addr_size);

    if (status == -1) {
        fprintf(
            stderr,
            "Error: Getting the info about the next assigned socket failed");
        return status;
    }

    return status;
}

void *RunClientThread(void *_conn_info) {
    struct ConnectionInfo *conn_info = (struct ConnectionInfo *)_conn_info;

    int status = SetupPlayerSockets(conn_info);
    if (status != 0) {
        printf("ERROR: Failed to setup the player socket\n");
        close(conn_info->udp_socket_fd);
        close(conn_info->socket_fd);
        free(conn_info);
        pthread_exit(NULL);
    }

    char angle_buf[1 + 1 + 4];

    printf("starting loop");
    while (1) {
        status = recv(conn_info->socket_fd, angle_buf, sizeof angle_buf, 0);

        if (status == 0) {
            printf("INFO: client %d disconnected\n", conn_info->socket_fd);
            break;
        }
        if (status == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("INFO: client %d is still connected, the socket has no "
                       "data\n",
                       conn_info->socket_fd);
            } else {
                perror("ERROR: thread TCP recv");
                break;
            }
        }

        printf("1\n");
        int bytes =
            recvfrom(conn_info->udp_socket_fd, angle_buf, sizeof angle_buf, 0,
                     (struct sockaddr *)&conn_info->udp_their_addr,
                     &conn_info->udp_their_addr_size);

        switch (angle_buf[0]) {
        case CLIENT_PLAYER_DATA_BROADCAST: {
            const int angle_message_length = 1 + 2 + 4;
            if (bytes < angle_message_length) {
                printf("WARN: expected %d bytes when receiving "
                       "CLIENT_PLAYER_DATA_BROADCAST, received %d\n",
                       angle_message_length, bytes);
                break;
            }
            int *angle = (int *)&angle_buf[3];
            SetPlayerAngle(conn_info->player.index, *angle);

            break;
        }
        case GAME_STARTED: {
            SetGameStarted(true);
            break;
        }
        default: {
            printf(
                "WARN: read %d bytes from a client with an unknown message\n",
                bytes);
        }
        }
    }

    printf("INFO: exiting thread with socket: %d", conn_info->socket_fd);
    close(conn_info->udp_socket_fd);
    close(conn_info->socket_fd);
    free(conn_info);

    pthread_exit(NULL);
}

int n_clients;
struct ConnectionInfo clients[MAX_PLAYERS];

void BroadcastGameData(struct GameState game_state) {
    for (int i = 0; i < n_clients; i++) {
        struct ConnectionInfo conn_info = clients[i];

        struct GameStateBroadcast game_state_broadcast;
        game_state_broadcast.packet_type = SERVER_GAME_DATA_BROADCAST;
        game_state_broadcast.packet_size = sizeof(game_state_broadcast);
        game_state_broadcast.game_state = game_state;

        sendto(conn_info.udp_socket_fd, (void *)&game_state_broadcast,
               sizeof((game_state_broadcast)), 0,
               (struct sockaddr *)(&(conn_info.udp_their_addr)),
               conn_info.udp_their_addr_size);
    }
}

void AddClient(struct ConnectionInfo conn_info) {
    clients[n_clients] = conn_info;
    n_clients += 1;
}
