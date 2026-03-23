#include "raylib.h"
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
    GAME_STARTED,
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

    SetTargetFPS(60);
    int angle_deg = 90;

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

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
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
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

    freeaddrinfo(servinfo);

    if ((numbytes = sendto(sockfd, buf, 3, 0, p->ai_addr, p->ai_addrlen)) ==
        -1) {
        perror("ERROR: sendto()");
        exit(1);
    }
    printf("INFO: sent INITIAL_CONNECTION message\n");

    sleep(3);

    buf[0] = GAME_STARTED;
    buf[1] = 0;
    buf[2] = 3;

    if ((numbytes = sendto(sockfd, buf, 3, 0, p->ai_addr, p->ai_addrlen)) ==
        -1) {
        perror("ERROR: sendto()");
        exit(1);
    }
    printf("INFO: sent GAME_STARTED message\n");

    while (!WindowShouldClose()) {
        numbytes =
            recvfrom(sockfd, buf, sizeof buf, 0, p->ai_addr, &p->ai_addrlen);
        printf("INFO: received %d bytes: %s\n", numbytes, buf);

        switch (buf[0]) {
        case SERVER_GAME_DATA_BROADCAST: {
            char _n_players[4] = {buf[3], buf[4], buf[5], buf[6]};
            int *n_players = (int *)_n_players;

            printf("There are %d players in the game\n", *n_players);
            break;
        }
        }

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

    close(sockfd);

    CloseWindow();

    return 0;
}
