/*! \file main.h
 *  \brief Packet information for networking
 */

/*
 * Packet format (UDP):
 * Byte 0: packet type
 * Byte 1: packet size in bytes
 * Byte 2 plus: data body
 * */
typedef enum PacketID {
    INITIAL_CONNECTION = 0,
    GAME_STARTED,
    SERVER_GAME_DATA_BROADCAST,
    CLIENT_PLAYER_DATA_BROADCAST,
} PacketID;

struct ClientPlayer {
    int x;
    int y;
    int radius;
    char *name;
};

struct ClientFruit {
    int x;
    int y;
    int radius;
};

struct GameState {
    int n_players;
    struct ClientPlayer *players;

    int n_fruits;
    struct ClientFruit *fruits;
};

struct ClientInput {
    int angle_deg;
};
