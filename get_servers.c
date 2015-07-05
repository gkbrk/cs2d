#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libcs2d.h"

#include <libxml/parser.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in *servers;
    int ips = cs2d_get_servers(&servers);

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    if (argc > 1 && strcmp(argv[1], "xml") == 0){
        printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        printf("<servers>\n");
    }else if (argc > 1 && strcmp(argv[1], "libxml") == 0){
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "servers");
        xmlDocSetRootElement(doc, root_node);
    }

    for (int i=0;i<ips;i++){
        struct cs2dServer *server = cs2d_get_serverinfo(servers[i]);
        if (server != NULL){
            if (argc > 1 && strcmp(argv[1], "lensplit") == 0){
                printf("%lu;%s|%lu;%s|%d|%d|%d\n", strlen(server->name), server->name, strlen(server->mapName), server->mapName, server->players, server->maxPlayers, server->bots);
            }else if (argc > 1 && strcmp(argv[1], "xml") == 0){
                printf("    <server>\n");
                printf("        <name><![CDATA[%s]]></name>\n", server->name);
                printf("        <mapName><![CDATA[%s]]></mapName>\n", server->mapName);
                printf("        <playerCount>%d</playerCount>\n", server->players);
                printf("        <maxPlayers>%d</maxPlayers>\n", server->maxPlayers);
                printf("        <botCount>%d</botCount>\n", server->bots);
                printf("        <players>\n");
                char **players; 
                int playerCount = cs2d_get_players(server->address, &players);
                for (int i=0;i<playerCount;i++){
                    printf("            <player><![CDATA[%s]]></player>\n", players[i]);
                }
                printf("        </players>\n");
                printf("    </server>\n");
            }else if (argc > 1 && strcmp(argv[1], "libxml") == 0){
                char buffer[255];
                xmlNodePtr serverNode = xmlNewChild(root_node, NULL, BAD_CAST "server", NULL);
                xmlNewChild(serverNode, NULL, BAD_CAST "name", BAD_CAST server->name);
                xmlNewChild(serverNode, NULL, BAD_CAST "mapName", BAD_CAST server->mapName);

                sprintf(buffer, "%d", server->players);
                xmlNewChild(serverNode, NULL, BAD_CAST "playerCount", BAD_CAST buffer);

                sprintf(buffer, "%d", server->maxPlayers);
                xmlNewChild(serverNode, NULL, BAD_CAST "maxPlayers", BAD_CAST buffer);

                sprintf(buffer, "%d", server->bots);
                xmlNewChild(serverNode, NULL, BAD_CAST "botCount", BAD_CAST buffer);

                xmlNodePtr playersNode = xmlNewChild(serverNode, NULL, BAD_CAST "players", NULL);
                char **players;
                int playerCount = cs2d_get_players(server->address, &players);
                for (int i=0;i<playerCount;i++){
                    xmlNewChild(playersNode, NULL, BAD_CAST "player", BAD_CAST players[i]);
                }
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
    }else if (argc > 1 && strcmp(argv[1], "libxml") == 0){
        xmlChar *xmlbuff;
        int buffersize;
        xmlDocDumpMemory(doc, &xmlbuff, &buffersize);
        printf("%s", (char *)xmlbuff);
    }
}
