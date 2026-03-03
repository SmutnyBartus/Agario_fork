/*! \file server.h
 *  \brief Contains game logic
 */

#ifndef GAME_H
#define GAME_H

void MainLoop();
void GetPlayerAngles(int *player_angles, int *n);
void ProcessPlayerMovement();
void ProcessCollisions();

#endif // !GAME_H
