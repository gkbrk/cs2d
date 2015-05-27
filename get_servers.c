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
            if (server->bots > 0){
                printf("%s | %s [%d/%d] (%d bots)\n", server->name, server->mapName, server->players, server->maxPlayers, server->bots);
            }else{
                printf("%s | %s [%d/%d]\n", server->name, server->mapName, server->players, server->maxPlayers);
            }

            if (server->fogOfWar){
                printf("%s: Fog of war enabled\n", server->name);
            }

            if (server->gamemode == CONSTRUCTION){
                printf("Construction server found! %s\n", server->name);
            }

            char **players;
            int num_players = cs2d_get_players(server->address, &players);
            for (int i=0;i<num_players;i++){
                //printf("Player %d: %s ", i, players[i]);
                free(players[i]);
            }
            free(players);
        }
        free(server);
    }
    free(servers);
}
