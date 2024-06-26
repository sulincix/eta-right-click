#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <arpa/inet.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdint.h>
#include "socket.h"
#include "timeout.h"


#define die(str, args...) { \
    perror(str); exit(EXIT_FAILURE); \
}

int udp_socket;

int fd_rel;
int fd_abs;

int mode;

extern int action_id;

void update_abs_info(int fd, int min_x, int min_y, int max_x, int max_y)
{
    struct uinput_abs_setup abs_setup;

    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_X;
    abs_setup.absinfo.value = 0;
    abs_setup.absinfo.minimum = min_x;
    abs_setup.absinfo.maximum = max_x;
    abs_setup.absinfo.fuzz = 0;
    abs_setup.absinfo.flat = 0;
    abs_setup.absinfo.resolution = 400;
    if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
        die("error: UI_ABS_SETUP ABS_X");

    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_Y;
    abs_setup.absinfo.value = 0;
    abs_setup.absinfo.minimum = min_y;
    abs_setup.absinfo.maximum = max_y;
    abs_setup.absinfo.fuzz = 0;
    abs_setup.absinfo.flat = 0;
    abs_setup.absinfo.resolution = 400;
    if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
        die("error: UI_ABS_SETUP ABS_Y");

    memset(&abs_setup, 0, sizeof(abs_setup));
    abs_setup.code = ABS_PRESSURE;
    abs_setup.absinfo.value = 0;
    abs_setup.absinfo.minimum = 0;
    abs_setup.absinfo.maximum = INT16_MAX;
    abs_setup.absinfo.fuzz = 0;
    abs_setup.absinfo.flat = 0;
    abs_setup.absinfo.resolution = 0;
    if (ioctl(fd, UI_ABS_SETUP, &abs_setup) < 0)
        die("error: UI_ABS_SETUP ABS_PRESSURE");

}

void init_device(int fd, int abs)
{
   	// enable synchronization
    if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0)
        die("error: ioctl UI_SET_EVBIT EV_KEY");

   	// enable 1 button
    if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl UI_SET_EVBIT EV_KEY");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH) < 0)
        die("error: ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        die("error: ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE) < 0)
        die("error: ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) < 0)
        die("error: ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_TOOL_PEN) < 0)
        die("error  ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS) < 0)
        die("error: ioctl UI_SET_KEYBIT");
    if (ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS2) < 0)
        die("error: ioctl UI_SET_KEYBIT");

    if (abs == 0)
    {
       	// Enable relative events
        if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
            die("error: ioctl UI_SET_EVBIT EV_REL");

       	// Enable specific relative events
        if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
            die("error: ioctl UI_SET_RELBIT REL_X");
        if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
            die("error: ioctl UI_SET_RELBIT REL_Y");
    }
    else
    {
       	// enable 2 main axes + pressure (absolute positioning)
        if (ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
            die("error: ioctl UI_SET_EVBIT EV_ABS");
        if (ioctl(fd, UI_SET_ABSBIT, ABS_X) < 0)
            die("error: ioctl UI_SETEVBIT ABS_X");
        if (ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
            die("error: ioctl UI_SETEVBIT ABS_Y");
        if (ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE) < 0)
            die("error: ioctl UI_SETEVBIT ABS_PRESSURE");

        update_abs_info(fd_abs, 0, 0, INT16_MAX, INT16_MAX);
    }

    struct uinput_setup setup;

    memset(&setup, 0, sizeof(setup));
    snprintf(setup.name, UINPUT_MAX_NAME_SIZE, "Amogus Device");
    setup.id.bustype = BUS_VIRTUAL;
    setup.id.vendor = 0x1453;
    setup.id.product = 0x1453 + abs;
    setup.id.version = 2 + abs;
    setup.ff_effects_max = 0;

    if (ioctl(fd, UI_DEV_SETUP, &setup) < 0)
        die("error: UI_DEV_SETUP");

    if (ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");
}

void event_sync(int fd){
    struct input_event ev;
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    if (write(fd, &ev, sizeof(ev)) < 0)
        die("error: write()");
}

static bool pressed = false;
static bool lock = false;
static bool press_lock = false;
void* check_lock(void* args){
    if(pressed) {
        puts("lock");
        lock = true;
    }
    return NULL;
}

void* check_right(void* args){
    if(!pressed && lock) { // long press & release
        struct input_event ev;
        ev.type = EV_KEY;
        ev.code = BTN_RIGHT;
        ev.value = 1;
        write(fd_rel, &ev, sizeof(ev));
        event_sync(fd_rel);
        usleep(100);
        ev.value = 0;
        write(fd_rel, &ev, sizeof(ev));
        event_sync(fd_rel);
        lock = false;
        puts("right");
    } else if(pressed){ // long press && wait
        struct input_event ev;
        ev.type = EV_KEY;
        ev.code = BTN_LEFT;
        ev.value = 1;
        write(fd_rel, &ev, sizeof(ev));
        event_sync(fd_rel);
        press_lock = true;
        puts("select");
    }
    return NULL;
}
void* check_left(void* args){
    if(!pressed) {
        struct input_event ev;
        ev.type = EV_KEY;
        ev.code = BTN_LEFT;
        ev.value = 1;
        write(fd_rel, &ev, sizeof(ev));
        event_sync(fd_rel);
        usleep(100);
        ev.value = 0;
        write(fd_rel, &ev, sizeof(ev));
        event_sync(fd_rel);
        puts("left");
    }
    return NULL;
}

uint64_t cur;
void send_event(socket_data data) {
    struct input_event ev;
    ev.type = data.ev_type;
    ev.code = data.ev_code;
    ev.value = data.ev_value;

    if (ev.type == EV_KEY && (ev.code == BTN_LEFT)){
        pressed = ev.value;
        if(ev.value == 1){
            action_id++;
            printf("press\n");
            timeout_add(2000, (void*)check_right, NULL);
            timeout_add(750, (void*)check_lock, NULL);
            timeout_add(300, (void*)check_left, NULL);
        } else {
            if(micro_time() - cur < 300){
                pressed = true;
                check_left(NULL); // left click
                pressed = false;
            }
            printf("release\n");
        }
        cur = micro_time();
        if(!press_lock){
            press_lock = false;
            return;
        }
    }

    if (ev.type == EV_ABS) {
        if (ev.code == ABS_X)
            ev.value = (ev.value - data.min_x) *INT16_MAX / (data.max_x - data.min_x);
        if (ev.code == ABS_Y)
            ev.value = (ev.value - data.min_y) *INT16_MAX / (data.max_y - data.min_y);
        mode = EV_ABS;
    } else if (ev.type == EV_REL)  {
        mode = EV_REL;
    }

    if (mode == EV_ABS) {
        printf("abs: %d %d %d\n", ev.type, ev.code, ev.value);
        if (write(fd_abs, &ev, sizeof(ev)) < 0)
            die("error: write()");
    } else {
        printf("rel: %d %d %d\n", ev.type, ev.code, ev.value);
        if (write(fd_rel, &ev, sizeof(ev)) < 0)
            die("error: write()");
    }

    if (write(fd_rel, &ev, sizeof(ev)) < 0)
        die("error: write()");
    if (write(fd_abs, &ev, sizeof(ev)) < 0)
        die("error: write()");
    event_sync(fd_rel);
    event_sync(fd_abs);

}


int server_main()
{
    socket_init();

    if ((fd_rel = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0)
        die("error: open");

    if ((fd_abs = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0)
        die("error: open");

    init_device(fd_rel, 0);
    init_device(fd_abs, 1);

    while (1)
    {
        socket_data data = socket_read();
        send_event(data);
    }

    return 0;
}
