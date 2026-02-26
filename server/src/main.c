/*! \file main.c
 *  \brief Test - a documented main file
 */

#include "add.h"
#include <stdio.h>

/*!
 *  Test - this is the main function
 *  @param argc The amount of arguments
 */
int main(int argc, char *argv[]) {
    add(1, 2);
    printf("starting\n");
    return 0;
}
