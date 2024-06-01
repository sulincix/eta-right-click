#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "socket.h"

int soc_server_fd;
struct sockaddr_un server_from;
struct sockaddr_un server_addr;
socklen_t fromlen;
void socket_init() {
    int ok = 1;
    fromlen = sizeof(server_from);

    if ((soc_server_fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        ok = 0;
    }

    bind(soc_server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (ok) {
        memset( & server_from, 0, sizeof(server_addr));
        server_from.sun_family = AF_UNIX;
        strcpy(server_from.sun_path, SERVER_SOCK_FILE);
        unlink(SERVER_SOCK_FILE);
        if (bind(soc_server_fd, (struct sockaddr * ) & server_from, sizeof(server_from)) < 0) {
            perror("bind");
            ok = 0;
        }
    }
    chmod(SERVER_SOCK_FILE, S_IWUSR | S_IWGRP | S_IWOTH);
}
int socket_destroy() {

    if (soc_server_fd >= 0) {
        close(soc_server_fd);
    }

    return 0;
}

socket_data socket_read() {
    socket_data data;
    recvfrom(soc_server_fd, &data, sizeof(socket_data), 0, (struct sockaddr * ) & server_from, & fromlen);
    return data;
}
