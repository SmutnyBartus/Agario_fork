/*! \file server.h
 *  \brief Contains game logic
 */

#include <stdbool.h>

#ifndef GAME_H
#define GAME_H

#define STARTING_RADIUS 10

#define MAP_HEIGHT 10000
#define MAP_WIDTH 10000

#define SPEED 100
#define DELTA_TIME_S 0.01

#define FRUIT_REWARD 10
#define N_FRUITS 100
#define FRUIT_RADIUS 10

struct Position2D {
    int x;
    int y;
};

struct Player {
    int index;
    int radius;
    struct Position2D pos;
};

/*
 * A function adding a player to the game logic. It returns the Player structure
 * with the newly created player, which should be passed to the appropriate
 * thread
 * @return The index of the newly created player
 */
struct Player AddPlayer();

void MainLoop();
void ProcessPlayerMovement();
void ProcessCollisions();
void SendGameData();
bool IsGameStarted();

/*
 * A function allowing threads to write their received player angles (in
 * degrees) concurrently
 * @param index The index of the player passed to the thread on initialization
 * @param angle_deg The player angle in degrees received from the client
 */
void SetPlayerAngle(int index, int angle_deg);

#endif // !GAME_H
