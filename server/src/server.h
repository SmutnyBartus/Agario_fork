/*! \file server.h
 *  \brief Contains all core server functions, including multithreaded tasks
 */

#ifndef SERVER_H
#define SERVER_H
#define PORT "8080"

#include "game.h"
#include "packets.h"
#include <sys/socket.h>

struct ConnectionInfo {
    int udp_socket_fd;
    struct sockaddr_storage udp_their_addr;
    socklen_t udp_their_addr_size;

    struct Player player;
};

// struct GameState {
//     int n_players;
//     struct ClientPlayer players[5];
//
//     int n_fruits;
//     struct ClientFruit fruits[10];
// };

struct __attribute__((packed)) GameStateBroadcast {
    char packet_type;
    int packet_size;
    int player_index;
    struct GameState game_state;
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

void BroadcastGameData(struct GameState game_state);

void AddClient(struct ConnectionInfo conn_info);

void *GetInAddr(struct sockaddr *sa);

void InitFruits();

int SetupPlayerSockets(struct ConnectionInfo *conn_info);

#endif // !SERVER_H
