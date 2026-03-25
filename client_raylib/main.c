#include "raylib.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT "8080"
#define MAXDATASIZE 100

const int screenWidth = 800;
const int screenHeight = 450;

typedef enum PacketID {
    INITIAL_CONNECTION = 0,
    SERVER_GAME_DATA_BROADCAST,
    CLIENT_PLAYER_DATA_BROADCAST,
} PacketID;

struct ClientPlayer {
    int x;
    int y;
    int radius;
    char *name;
};

struct ClientFruit {
    int x;
    int y;
    int radius;
};

struct GameState {
    int n_players;
    struct ClientPlayer *players;

    int n_fruits;
    struct ClientFruit *fruits;
};

struct ClientInput {
    int angle_deg;
};

struct GameStateBroadcast {
    char packet_type;
    char packet_size;
    struct GameState game_state;
};

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "Agario");

    struct GameState game_state = {};

    SetTargetFPS(10);
    int angle_deg = 90;

    int udp_socket;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    struct sockaddr addr = {};
    socklen_t len = sizeof(addr);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    rv = getaddrinfo(HOST, PORT, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "ERROR: getaddrinfo(): %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((udp_socket =
                 socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("ERROR: socket()");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "ERROR: failed to create socket\n");
        return 2;
    }

    char buf[1000] = {INITIAL_CONNECTION, 0, 3};

    if ((numbytes = sendto(udp_socket, buf, 3, 0, p->ai_addr, p->ai_addrlen)) ==
        -1) {
        perror("ERROR: sendto()");
        exit(1);
    }
    printf("INFO: sent INITIAL_CONNECTION message\n");

    struct __attribute__((packed)) {
        char type;
        short length;
    } buf2;
    numbytes = recvfrom(udp_socket, &buf2, sizeof(buf2), 0, &addr, &len);
    printf("INFO: received %d bytes, message type is %d, len is %d\n", numbytes,
           buf2.type, buf2.length);

    fcntl(udp_socket, F_SETFL, O_NONBLOCK);
    while (!WindowShouldClose()) {

        struct __attribute__((packed)) {
            char type;
            short length;
            int angle_deg;
        } angle_buf;

        angle_buf.type = CLIENT_PLAYER_DATA_BROADCAST;
        angle_buf.length = sizeof(angle_buf);
        angle_buf.angle_deg = angle_deg;

        assert(angle_buf.length == sizeof(angle_buf));

        printf("INFO: sending angle: %d\n", angle_buf.angle_deg);

        numbytes =
            sendto(udp_socket, &angle_buf, sizeof(angle_buf), 0, &addr, len);
        if (numbytes == -1) {
            perror("ERROR: angle_deg sendto()");
            break;
        }

        // numbytes = recvfrom(udp_socket, buf, sizeof buf, 0,
        //                     (struct sockaddr *)&p->ai_addr, &p->ai_addrlen);
        // if (numbytes == 0) {
        //     printf("INFO: server disconnected\n");
        //     break;
        // } else if (numbytes == -1) {
        //     if (errno == EAGAIN || errno == EWOULDBLOCK) {
        //         printf("INFO: no data received from server\n");
        //     } else {
        //         perror("ERROR: thread UDP recv");
        //         break;
        //     }
        // } else {
        //     printf("INFO: received %d bytes, message type is %d\n", numbytes,
        //            buf[0]);
        // }

        // switch (buf[0]) {
        // case SERVER_GAME_DATA_BROADCAST: {
        //
        //     char _n_players[4] = {buf[3], buf[4], buf[5], buf[6]};
        //     int *n_players = (int *)_n_players;
        //
        //     printf("INFO: There are %d players in the game\n", *n_players);
        //     break;
        // }
        // }

        // Vector2 mouse_pos = GetMousePosition();
        // Vector2 center = {(float)screenWidth / 2, (float)screenHeight / 2};
        //
        // float dx = mouse_pos.x - center.x;
        // float dy = mouse_pos.y - center.y;
        //
        // float angle_rad = atan2(dy, dx);
        // angle_deg = RAD2DEG * angle_rad / 10;
        // printf("angle: %d\n", angle_deg);
        //
        if (IsKeyDown(KEY_RIGHT))
            angle_deg = 0;
        if (IsKeyDown(KEY_LEFT))
            angle_deg = 180;
        if (IsKeyDown(KEY_UP))
            angle_deg = 90;
        if (IsKeyDown(KEY_DOWN))
            angle_deg = 270;
        printf("Angle: %d\n", angle_deg);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);

        for (int i = 0; i < game_state.n_players; i++) {
            Vector2 pos = {game_state.players[i].x, game_state.players[i].y};
            DrawCircleV(pos, game_state.players[i].radius, MAROON);
        }

        for (int i = 0; i < game_state.n_fruits; i++) {
            Vector2 pos = {game_state.fruits[i].x, game_state.players[i].y};
            DrawCircleV(pos, 10, MAROON);
        }

        EndDrawing();
    }

    freeaddrinfo(servinfo);
    close(udp_socket);

    CloseWindow();

    return 0;
}
