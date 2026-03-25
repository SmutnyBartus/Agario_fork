/*! \file main.h
 *  \brief Packet information for networking
 */

/*
 * Packet format (UDP):
 * Byte 0: packet type
 * Bytes 1-2: packet size in bytes
 * Byte 3 plus: data body
 * */

typedef enum PacketID {
    INITIAL_CONNECTION = 0,
    SERVER_GAME_DATA_BROADCAST,
    CLIENT_PLAYER_DATA_BROADCAST,
} PacketID;

struct ClientPlayer {
    struct Position2D pos;
    int radius;
    char *name;
};

struct ClientFruit {
    struct Position2D pos;
    int radius;
};

struct GameState {
    int n_players;
    struct Player players[5];

    int n_fruits;
    struct Fruit fruits[N_FRUITS];
};

struct ClientInput {
    int angle_deg;
};
