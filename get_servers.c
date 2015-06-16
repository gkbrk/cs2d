#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libcs2d.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in *servers;
    int ips = cs2d_get_servers(&servers);

    if (argc > 1 && strcmp(argv[1], "xml") == 0){
        printf("<?xml version=\"1.0\"?>\n");
        printf("<servers>\n");
    }

    for (int i=0;i<ips;i++){
        struct cs2dServer *server = cs2d_get_serverinfo(servers[i]);
        if (server != NULL){
            if (argc > 1 && strcmp(argv[1], "lensplit") == 0){
                printf("%lu;%s|%lu;%s|%d|%d|%d\n", strlen(server->name), server->name, strlen(server->mapName), server->mapName, server->players, server->maxPlayers, server->bots);
            }else if(argc > 1 && strcmp(argv[1], "xml") == 0){
                printf("    <server>\n");
                printf("        <name>%s</name>\n", server->name);
                printf("        <mapName>%s</mapName>\n", server->mapName);
                printf("        <playerCount>%d</playerCount>\n", server->players);
                printf("        <maxPlayers>%d</maxPlayers>\n", server->maxPlayers);
                printf("        <botCount>%d</botCount>\n", server->bots);
                printf("        <players>\n");
                char **players; 
                int playerCount = cs2d_get_players(server->address, &players);
                for (int i=0;i<playerCount;i++){
                    printf("            <player>%s</player>\n", players[i]);
                }
                printf("        </players>\n");
                printf("    </server>\n");
            }else{
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
            }
        }
        free(server);
    }
    free(servers);
    if (argc > 1 && strcmp(argv[1], "xml") == 0){
        printf("</servers>\n");
    }
}
