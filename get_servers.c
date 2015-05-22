#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libcs2d.h"

int main() {
    struct sockaddr_in *servers;
    int ips = cs2d_get_servers(&servers);

    for (int i=0;i<ips;i++){
        struct cs2dServer *server = cs2d_get_serverinfo(servers[i]);
        if (server != NULL){
            printf("Got response: %s | %s [%d/%d]\n", server->name, server->mapName, server->players, server->maxPlayers);
        }
        free(server);
    }
    free(servers);
}
