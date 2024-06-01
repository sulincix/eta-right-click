extern void socket_init();
extern int* socket_read();
extern void client_init();
extern void client_send(int* data);

#define SERVER_SOCK_FILE "/dev/amogus-mouse"
#define SOCKET_BUFFER_LENGTH 4
