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

struct Player {
    int x;
    int y;
    int size;
    char *name;
};

struct GameState {
    int n_players;
    struct Player *players;

    int n_fruits;
    struct Fruit *fruits;
};

struct ClientInput {
    int angle_deg;
};
