#pragma once

#include <arpa/inet.h>

#define CS2D_TIMEOUT 100

struct cs2dServer{
    char name[100];
    char mapName[100];
    int players;
    int maxPlayers;
};

typedef struct cs2dServer cs2dServer;

extern void cs2d_bind_socket(int socket_descriptor);
extern void cs2d_request_ips(int socket_descriptor);
extern struct cs2dServer *cs2d_get_serverinfo(struct sockaddr_in socket);