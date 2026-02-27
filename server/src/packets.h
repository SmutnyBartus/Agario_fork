/*! \file main.h
 *  \brief Packet information for networking
 */

typedef enum PacketID {
    INITIAL_CONNECTION = 0,
    SERVER_GAME_DATA_BROADCAST,
    CLIENT_PLAYER_DATA_BROADCAST,
} PacketID;
