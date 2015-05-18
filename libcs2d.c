#include "libcs2d.h"

#include <stdlib.h>
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
    int loc = 5;
    if (length > 0){
        struct cs2dServer *sv = malloc(sizeof(struct cs2dServer));

        int name_len = response[loc];
        loc++;
        strncpy(sv->name, &response[loc], name_len);
        sv->name[name_len] = '\0';
        loc += name_len;

        int map_len = response[loc];
        loc++;
        strncpy(sv->mapName, &response[loc], map_len);
        sv->mapName[map_len] = '\0';
        loc += map_len;

        sv->players = response[loc];
        loc++;
        sv->maxPlayers = response[loc];

        char ip_text[500];
        inet_ntop(AF_INET, &(server.sin_addr), ip_text, 500);
        return sv;
    }else{

        return NULL;
    }
}
