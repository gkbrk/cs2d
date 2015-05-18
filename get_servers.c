#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libcs2d.h"

int main() {
    struct sockaddr_in *servers = cs2d_get_servers();
    int ips = servers[0].sin_port;
    for (int i=1;i<=ips;i++){
        struct cs2dServer *server = cs2d_get_serverinfo(servers[i]);
        if (server != NULL){
            printf("%s\n", server->name);
        }
        free(server);
    }
    free(servers);
}
