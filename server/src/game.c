#include "game.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

struct Position2D {
    int x;
    int y;
};

struct Player {
    int index;
    int radius;
    struct Position2D pos;
};
struct Player players[MAX_PLAYERS];

int n_players = 0;
struct Player AddPlayer() {
    struct Player player;
    player.radius = STARTING_RADIUS;
    player.index = n_players;
    player.pos.x = rand() % MAP_WIDTH;
    player.pos.y = rand() % MAP_HEIGHT;

    n_players++;
    return player;
}

#define CACHE_PADDING 64
int player_angles_deg[MAX_PLAYERS * CACHE_PADDING] = {0};

pthread_mutex_t player_angle_mutex = PTHREAD_MUTEX_INITIALIZER;
void SetPlayerAngle(int index, int angle_deg) {
    pthread_mutex_lock(&player_angle_mutex);
    player_angles_deg[index * CACHE_PADDING] = angle_deg;
    pthread_mutex_unlock(&player_angle_mutex);
}

/*
 * A helper function to get the player angle in degrees. Not thread safe
 * @return The player angle in degrees
 */
int GetPlayerAngle(int index) {
    return player_angles_deg[index * CACHE_PADDING];
}

void MainLoop() {
    while (1) {
        ProcessPlayerMovement();
        ProcessCollisions();
        BroadcastGameData();
    }
}

void ProcessPlayerMovement() {
    pthread_mutex_lock(&player_angle_mutex);

    for (int i = 0; i < n_players; i++) {
        float angle_rad = (float)GetPlayerAngle(i) * M_PI / 180;
        players[i].pos.x += (int)SPEED * DELTA_TIME_S * cos(angle_rad);
        players[i].pos.y += (int)SPEED * DELTA_TIME_S * sin(angle_rad);

        if (players[i].pos.x > MAP_WIDTH) {
            players[i].pos.x = MAP_WIDTH;
        }
        if (players[i].pos.x < 0) {
            players[i].pos.x = 0;
        }

        if (players[i].pos.y > MAP_HEIGHT) {
            players[i].pos.y = MAP_HEIGHT;
        }
        if (players[i].pos.y < 0) {
            players[i].pos.y = 0;
        }
    }

    pthread_mutex_unlock(&player_angle_mutex);
}

void ProcessCollisions() {}

void BroadcastGameData() {}
