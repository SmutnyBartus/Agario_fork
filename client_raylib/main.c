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

    // połącz tcp
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            perror("client: socket");
            continue;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
                  sizeof s);
        printf("client: attempting connection to %s\n", s);

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
              sizeof s);
    printf("client: connected to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n", buf);

    while (!WindowShouldClose()) {
        // odbierz udp gamestatu

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

    // zamknij tcp
    close(sockfd);

    CloseWindow();

    return 0;
}
