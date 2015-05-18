#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "libcs2d.h"

int main() {
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_desc == -1){
        perror("Could not create socket.");
        exit(1);
    }

    cs2d_bind_socket(sock_desc);

    cs2d_request_ips(sock_desc);

    unsigned char response[1024];
    recvfrom(sock_desc, response, 1024, 0, NULL, NULL);
    int ips = response[3];
    printf("There are %d servers in total.\n", ips);

    int loc = 5;
    for (int i=0;i<ips;i++){
        char ip[500];
        sprintf(ip, "%d.%d.%d.%d", response[loc+3], response[loc+2], response[loc+1], response[loc]);
        int port = response[loc+4] | response[loc+5] << 8;

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(ip);

        struct cs2dServer *sv = cs2d_get_serverinfo(server);
        printf("Checking %s:%d...      \r", ip, port);
        fflush(stdout);
        if (sv != NULL){
            printf("Got response: %s:%d | %d/%d - %s - %s\n",  ip, port, sv->players, sv->maxPlayers, sv->name, sv->mapName);
            free(sv);
        }

        loc += 6;
    }
}
