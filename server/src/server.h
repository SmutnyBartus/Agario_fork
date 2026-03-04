/*! \file server.h
 *  \brief Contains all core server functions, including multithreaded tasks
 */

#ifndef SERVER_H
#define SERVER_H

#include "packets.h"
#include <sys/socket.h>

struct ConnectionInfo {
    int socket_fd;
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;
};

/*
 *   A helper function that sets up a TCP socket and returns it
 *   @param port A string representing a port number the socket should bind to
 *   @return A TCP socket ready to accept() connections
 *
 */
int SetupMainSocket(const char *port);

/*
 *   The core worker function that runs on every thread. It is responsible for
 * collecting UDP packets from the threads and updating the data array
 *   @param socket_id The file descriptor of the socket the thread will accept
 * packets from
 *
 */
void *RunClientThread(void *conn_info);

void BroadcastGameData(struct GameState gamestate);

#endif // !SERVER_H
