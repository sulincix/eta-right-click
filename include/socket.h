
#define SERVER_SOCK_FILE "/dev/amogus-mouse"

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#define EVENT 0
#define UPDATE 1

typedef struct _socket_data{
    int action;
    const struct input_absinfo abs_info_x;
    const struct input_absinfo abs_info_y;
    int ev_code;
    int ev_type;
    int ev_value;
} socket_data;

extern void socket_init();
extern socket_data socket_read();
extern void client_init();
extern void client_send(socket_data data);
