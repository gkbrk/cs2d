#pragma once

#include <arpa/inet.h>

#define CS2D_TIMEOUT 500

struct cs2dServer{
    char name[256];
    char mapName[256];
    int players;
    int maxPlayers;
    struct sockaddr_in address;
};

typedef struct cs2dServer cs2dServer;

void cs2d_bind_socket(int socket_descriptor);
void cs2d_request_ips(int socket_descriptor);
int cs2d_get_servers(struct sockaddr_in **servers);
struct cs2dServer *cs2d_get_serverinfo(struct sockaddr_in socket);
