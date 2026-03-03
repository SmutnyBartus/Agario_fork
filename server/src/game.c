#include "game.h"

void MainLoop() {
    while (1) {
        GetPlayerAngles();
        ProcessPlayerMovement();
        ProcessCollisions();
    }
}
