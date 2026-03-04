#include "game.h"
#include "global_constants.h"
#include "server.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>

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

struct Fruit {
    int radius;
    struct Position2D pos;
};
struct Fruit fruits[N_FRUITS];

void InitFruits() {
    for (int i = 0; i < N_FRUITS; i++) {
        fruits[i].pos.x = rand() % MAP_WIDTH;
        fruits[i].pos.y = rand() % MAP_HEIGHT;

        fruits[i].radius = FRUIT_RADIUS;
    }
}

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

struct GameState game_state;

void MainLoop() {
    while (1) {
        ProcessPlayerMovement();
        ProcessCollisions();
        BroadcastGameData(game_state);
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

bool AreCirclesColliding(struct Position2D a_pos, int a_radius,
                         struct Position2D b_pos, int b_radius) {
    return abs((a_pos.x - b_pos.x) * (a_pos.x - b_pos.x) +
                   (a_pos.y - b_pos.y) * (a_pos.y - b_pos.y) <
               (a_radius + b_radius) * (a_radius + b_radius));
}

void ResolvePlayerCollision(struct Player *bigger, struct Player *smaller) {
    bigger->radius += smaller->radius;
    smaller->radius = STARTING_RADIUS;

    smaller->pos.x = rand() % MAP_WIDTH;
    smaller->pos.y = rand() % MAP_HEIGHT;
}

void ResolveFruitCollision(struct Player *player, struct Fruit *fruit) {
    player->radius += FRUIT_REWARD;

    fruit->pos.x = rand() % MAP_WIDTH;
    fruit->pos.y = rand() % MAP_HEIGHT;
}

void ProcessCollisions() {
    for (int i = 0; i < n_players; i++) {
        for (int j = 0; j < n_players; j++) {
            if (AreCirclesColliding(players[i].pos, players[i].radius,
                                    players[j].pos, players[j].radius)) {
                if (players[i].radius > players[j].radius) {
                    ResolvePlayerCollision(&players[i], &players[j]);
                } else if (players[i].radius < players[j].radius) {
                    ResolvePlayerCollision(&players[j], &players[i]);
                }
            }
        }
    }

    for (int i = 0; i < n_players; i++) {
        for (int j = 0; j < N_FRUITS; j++) {
            if (AreCirclesColliding(players[i].pos, players[i].radius,
                                    fruits[j].pos, fruits[j].radius)) {
                ResolveFruitCollision(&players[i], &fruits[j]);
            }
        }
    }
}

void SendGameData() {}

bool game_started = false;
pthread_mutex_t game_started_mutex = PTHREAD_MUTEX_INITIALIZER;

bool IsGameStarted() { return game_started; }

void SetGameStarted(bool is_game_started) {
    pthread_mutex_lock(&game_started_mutex);
    game_started = is_game_started;
    pthread_mutex_unlock(&game_started_mutex);
}
