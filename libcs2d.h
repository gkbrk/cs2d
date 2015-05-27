#pragma once

#include <arpa/inet.h>

#define CS2D_TIMEOUT 500

typedef enum {STANDARD, DEATHMATCH, TEAMDEATHMATCH, CONSTRUCTION} cs2dGamemode;

struct cs2dServer{
    char name[256];
    char mapName[256];
    int players;
    int maxPlayers;
    int bots;
    char fogOfWar;
    char luaScripts;
    char friendlyFire;
    char passwordProtected;
    cs2dGamemode gamemode;
    struct sockaddr_in address;
};

typedef struct cs2dServer cs2dServer;

void cs2d_bind_socket(int socket_descriptor);
void cs2d_request_ips(int socket_descriptor);
int cs2d_get_servers(struct sockaddr_in **servers);
struct cs2dServer *cs2d_get_serverinfo(struct sockaddr_in server);
int cs2d_get_players(struct sockaddr_in server, char ***players);
