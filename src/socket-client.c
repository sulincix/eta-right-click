#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "socket.h"

int soc_client_fd;
struct sockaddr_un client_from;
struct sockaddr_un client_addr;

void client_init() {
    soc_client_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, SERVER_SOCK_FILE);

    bind(soc_client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));
    connect(soc_client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));

}
void client_send(socket_data data){
    if (send(soc_client_fd, &data, sizeof(socket_data), 0) == -1) {
        perror("send");
        close(soc_client_fd);
        client_init();
    }
}
