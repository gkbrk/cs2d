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
            char **players;
            int num_players = cs2d_get_players(server->address, &players);
            for (int i=0;i<num_players;i++){
                printf("Player %d: %s ", i, players[i]);
                free(players[i]);
            }
            printf("\n");
            free(players);
        }
        free(server);
    }
    free(servers);
}
