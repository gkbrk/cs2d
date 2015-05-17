#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TIMEOUT 100

struct gameServer {
    char name[100];
    char mapname[100];
    int players;
    int maxonline;
};

void bind_socket(int socket_descriptor){
    struct sockaddr_in bind_address;
    bind_address.sin_family = AF_INET;
    bind_address.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_address.sin_port = htons(0);

    bind(socket_descriptor, (struct sockaddr *)&bind_address, sizeof(bind_address));
}

void request_ips(int socket_descriptor){
    struct hostent *usgn = gethostbyname("www.usgn.de");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(36963);
    bcopy(usgn->h_addr, &(server.sin_addr.s_addr), 4);

    char iprequest[] = {0x01, 0x00, 0x14, 0x01};
    sendto(socket_descriptor, iprequest, 4, 0, (struct sockaddr *)&server, sizeof(server));
}

struct gameServer *get_serverinfo(struct sockaddr_in server){
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    bind_socket(sock_desc);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUT*1000;

    setsockopt(sock_desc, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock_desc, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    char datarequest[] = {0x01, 0x00, 0x03, 0xa2, 0x55, 0xfb, 0x01, 0x7d, 0x00};
    sendto(sock_desc, datarequest, 9, 0, (struct sockaddr *)&server, sizeof(server));

    char response[1024];
    int length = recvfrom(sock_desc, response, 1023, 0, NULL, NULL);
    int loc = 5;
    if (length > 0){
        struct gameServer *sv = malloc(sizeof(struct gameServer));

        int name_len = response[loc];
        loc++;
        strncpy(sv->name, &response[loc], name_len);
        sv->name[name_len] = '\0';
        loc += name_len;

        int map_len = response[loc];
        loc++;
        strncpy(sv->mapname, &response[loc], map_len);
        sv->mapname[map_len] = '\0';
        loc += map_len;

        sv->players = response[loc];
        loc++;
        sv->maxonline = response[loc];

        char ip_text[500];
        inet_ntop(AF_INET, &(server.sin_addr), ip_text, 500);
        return sv;
    }else{

        return NULL;
    }
}

int main() {
    int sock_desc = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_desc == -1){
        perror("Could not create socket.");
        exit(1);
    }

    bind_socket(sock_desc);

    request_ips(sock_desc);

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

        struct gameServer *sv = get_serverinfo(server);
        printf("Checking %s:%d...      \r", ip, port);
        fflush(stdout);
        if (sv != NULL){
            printf("Got response: %s:%d | %d/%d - %s - %s\n",  ip, port, sv->players, sv->maxonline, sv->name, sv->mapname);
            free(sv);
        }

        loc += 6;
    }
}
