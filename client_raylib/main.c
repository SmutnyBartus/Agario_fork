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
#define N_FRUITS 20

const int screenWidth = 1000;
const int screenHeight = 1000;

struct Position2D {
    int x;
    int y;
};

struct Player {
    int index;
    int radius;
    struct Position2D pos;
};

struct Fruit {
    int radius;
    struct Position2D pos;
};


typedef enum PacketID {
    INITIAL_CONNECTION = 0,
    SERVER_GAME_DATA_BROADCAST,
    CLIENT_PLAYER_DATA_BROADCAST,
} PacketID;

struct ClientPlayer {
    struct Position2D pos;
    int radius;
    char *name;
};

struct ClientFruit {
    struct Position2D pos;
    int radius;
};

struct GameState {
    int n_players;
    struct Player players[5];

    int n_fruits;
    struct Fruit fruits[N_FRUITS];
};

struct ClientInput {
    int angle_deg;
};


struct GameStateBroadcast {
    char packet_type;
    char packet_size;
    struct GameState game_state;
};

int player_index;

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

        struct __attribute__((packed)) GameStateBroadcast{
            char packet_type;
            int packet_size;
            int player_index;
            struct GameState game_sate;
        } a;

        numbytes = recvfrom(udp_socket, &a, sizeof(a), 0,
                            (struct sockaddr *)&p->ai_addr, &p->ai_addrlen);
        if (numbytes == 0) {
            printf("INFO: server disconnected\n");
            break;
        } else if (numbytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("INFO: no data received from server\n");
            } else {
                perror("ERROR: thread UDP recv");
                break;
            }
        } else {
            printf("INFO: received %d bytes, message type is %d\n", numbytes,
                   a.packet_type);
        }

        switch (a.packet_type) {
        case SERVER_GAME_DATA_BROADCAST: {
            game_state = a.game_sate;
            player_index = a.player_index;
            printf("INFO: There are %d players in the game\n", a.game_sate.n_players);
            break;
        }
        }

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


        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        
        //Zrobiłem coś strasznego ale imie Szymona zostanie nieskalane....
        float cx = game_state.players[player_index].pos.x;
        float cy = game_state.players[player_index].pos.y;

        float dx = mouse_x - cx;
        float dy = mouse_y - cy;

        float angle = atan2f(dy, dx);

        angle_deg = -angle * (180.0f / PI);
        
        /*
        if (IsKeyDown(KEY_RIGHT))
            angle_deg = 0;
        if (IsKeyDown(KEY_LEFT))
            angle_deg = 180;
        if (IsKeyDown(KEY_UP))
            angle_deg = 90;
        if (IsKeyDown(KEY_DOWN))
            angle_deg = 270;
        */
        printf("Angle: %d\n", angle_deg);

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        for (int i = 0; i < game_state.n_players; i++) {
            Vector2 pos = {game_state.players[i].pos.x, game_state.players[i].pos.y};
            if(i == player_index)
                DrawCircleV(pos, game_state.players[i].radius, DARKBLUE);
            else
                DrawCircleV(pos, game_state.players[i].radius, BLACK);
        }

        for (int i = 0; i < game_state.n_fruits; i++) {
            Vector2 pos = {game_state.fruits[i].pos.x, game_state.fruits[i].pos.y};
            DrawCircleV(pos, 10, MAROON);
        }
        EndDrawing();
    }

    freeaddrinfo(servinfo);
    close(udp_socket);

    CloseWindow();

    return 0;
}
