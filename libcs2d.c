#include "libcs2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

void cs2d_bind_socket(int socket_descriptor){
    struct sockaddr_in bind_address;
    bind_address.sin_family = AF_INET;
    bind_address.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_address.sin_port = htons(0);

    bind(socket_descriptor, (struct sockaddr *)&bind_address, sizeof(bind_address));
}

void cs2d_request_ips(int socket_descriptor){
    struct hostent *usgn = gethostbyname("www.usgn.de");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(36963);
    bcopy(usgn->h_addr, &(server.sin_addr.s_addr), 4);

    char iprequest[] = {0x01, 0x00, 0x14, 0x01};
    sendto(socket_descriptor, iprequest, 4, 0, (struct sockaddr *)&server, sizeof(server));
}

int cs2d_get_servers(struct sockaddr_in **servers){
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    cs2d_bind_socket(sock_desc);
    cs2d_request_ips(sock_desc);

    unsigned char response[1024];
    recvfrom(sock_desc, response, 1024, 0, NULL, NULL);
    close(sock_desc);

    int ips = response[3];
    *servers = malloc(sizeof(struct sockaddr_in)*ips);

    int loc = 5;
    for (int i=0;i<ips;i++){
        char ip[500];
        sprintf(ip, "%d.%d.%d.%d", response[loc+3], response[loc+2], response[loc+1], response[loc]);
        int port = response[loc+4] | response[loc+5] << 8;

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(ip);

        (*servers)[i] = server;

        loc += 6;
    }

    return ips;
}

struct cs2dServer *cs2d_get_serverinfo(struct sockaddr_in server){
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    cs2d_bind_socket(sock_desc);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = CS2D_TIMEOUT*1000;

    setsockopt(sock_desc, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock_desc, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char datarequest[] = {0x01, 0x00, 0x03, 0xa2, 0x55, 0xfb, 0x01, 0x7d, 0x00};
    sendto(sock_desc, datarequest, 9, 0, (struct sockaddr *)&server, sizeof(server));

    char response[1024];
    int length = recvfrom(sock_desc, response, 1023, 0, NULL, NULL);
    close(sock_desc);

    int loc = 5;
    if (length > 0){
        struct cs2dServer *sv = malloc(sizeof(struct cs2dServer));

        int name_len = response[loc];
        loc++;
        memcpy(sv->name, &response[loc], 255);
        sv->name[name_len] = '\0';
        loc += name_len;

        int map_len = response[loc];
        loc++;
        memcpy(sv->mapName, &response[loc], 255);
        sv->mapName[map_len] = '\0';
        loc += map_len;

        sv->players = response[loc];
        loc++;
        sv->maxPlayers = response[loc];
        
        loc++;
        sv->fogOfWar = (response[loc] >> 3) & 1;

        for (int i=0;i<8;i++){
            //printf("Flag %d: %d ", i, (response[loc] >> i) & 1);
        }

        sv->address = server;
        return sv;
    }else{

        return NULL;
    }
}

int cs2d_get_players(struct sockaddr_in server, char ***players){
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    cs2d_bind_socket(sock_desc);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = CS2D_TIMEOUT*1000;

    setsockopt(sock_desc, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock_desc, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char datarequest[] = {0x01, 0x00, 0xfb, 0x05};
    sendto(sock_desc, datarequest, 4, 0, (struct sockaddr *)&server, sizeof(server));

    char response[4096];
    int length = recvfrom(sock_desc, response, 4095, 0, NULL, NULL);
    close(sock_desc);

    if (length > 0){
        int player_count = response[4];
        *players = malloc(sizeof(void*) * player_count);

        int pos = 5;
        for (int i=0;i<player_count;i++){ 
            pos++;
            int name_len = response[pos];
            pos++;

            char *name = malloc(256);
            memcpy(name, &response[pos], 255);
            name[name_len] = '\0';
            pos += name_len;
            (*players)[i] = name;

            pos += 9;
        }
        return player_count;
    }else{
        return 0;
    }
}
