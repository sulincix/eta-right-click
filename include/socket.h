
#define SERVER_SOCK_FILE "/dev/amogus-mouse"

#define EVENT 0
#define UPDATE 1

typedef struct _socket_data{
    int action;
    int ev_code;
    int ev_type;
    int ev_value;
} socket_data;

extern void socket_init();
extern socket_data socket_read();
extern void client_init();
extern void client_send(socket_data data);
