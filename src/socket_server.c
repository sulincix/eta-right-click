#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>

int soc_server_fd;
struct sockaddr_un server_from;
struct sockaddr_un server_addr;
socklen_t fromlen;

#define SOCKET_BUFFER_LENGTH 4

void socket_init() {
    int ok = 1;
    fromlen = sizeof(server_from);

    // Get the value of $XDG_RUNTIME_DIR
    char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (xdg_runtime_dir == NULL) {
        fprintf(stderr, "XDG_RUNTIME_DIR is not set.\n");
        exit(EXIT_FAILURE);
    }

    char socket_file_path[strlen(xdg_runtime_dir) + strlen("/eta-right-click") + 1];
    sprintf(socket_file_path, "%s/eta-right-click", xdg_runtime_dir);

    if ((soc_server_fd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        ok = 0;
    }

    bind(soc_server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (ok) {
        memset( & server_from, 0, sizeof(server_addr));
        server_from.sun_family = AF_UNIX;
        strcpy(server_from.sun_path, socket_file_path);
        unlink(socket_file_path);
        if (bind(soc_server_fd, (struct sockaddr * ) & server_from, sizeof(server_from)) < 0) {
            perror("bind");
            ok = 0;
        }
    }
    chmod(socket_file_path, S_IWUSR | S_IWGRP | S_IWOTH);
}

char * socket_read() {
    char * buff = malloc(SOCKET_BUFFER_LENGTH * sizeof(char));
    recvfrom(soc_server_fd, buff, SOCKET_BUFFER_LENGTH, 0, (struct sockaddr * ) & server_from, & fromlen);
    return buff;
}


