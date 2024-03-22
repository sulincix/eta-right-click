#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>

#define PATH_MAX 1024

int soc_client_fd;
struct sockaddr_un client_addr;

void socket_send(char* path, char* msg) {
    soc_client_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, path);

    bind(soc_client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));
    connect(soc_client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr));
    if (send(soc_client_fd, msg, strlen(msg) + 1, 0) == -1) {
        perror("send");
    }
    close(soc_client_fd);
}


void send_all(char* msg){
// Define the pattern
    const char *pattern = "/run/user/";

    // Open the directory
    DIR *dir = opendir(pattern);
    if (dir == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    // Read directory entries
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry matches the pattern
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s%s/eta-right-click", pattern, entry->d_name);

        if (access(full_path, F_OK) != -1) {
            socket_send(full_path, msg);
        }
    }

    // Close the directory
    closedir(dir);
}
