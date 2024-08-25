#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>

#include <pthread.h>
#include <string.h>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include <event.h>

#include "socket.h"


void listen_device(struct libevdev *dev) {
    int rc;
    (void)rc;
    socket_data data;
    const char *name = libevdev_get_name(dev);
    int vendor = libevdev_get_id_vendor(dev);
    struct libevdev_uinput *uidev;

    if(vendor == 0x1453){
        return;
    }

    printf("%s\n", name);

    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);

    libevdev_set_id_vendor(dev, 0x1453);
    libevdev_set_name(dev, "Amogus Device");

    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);

    while (1) {
        data.action = EVENT;

        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        data.ev_type = ev.type;
        data.ev_code = ev.code;
        data.ev_value = ev.value;
        if(ev.type == EV_KEY){
            printf("%d %d %d\n", ev.type, ev.code, ev.value);
            client_send(data);
        } else {
            rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
        }
    }
    libevdev_uinput_destroy(uidev);
}

pthread_t thread_id[256];
size_t max_dev;
void evdev_init() {
    int fd = 0;
    max_dev = 0;
    char devname[256] = "/dev/input/event";

    for (unsigned char i = 0; 1 ; ++i) {
        struct libevdev *dev = NULL;
        char devpath[1024];
        snprintf(devpath, sizeof(devpath), "%s%d", devname, i);
        fd = open(devpath, O_RDONLY);
        if (fd > 0) {
            libevdev_new_from_fd(fd, &dev);
            if (libevdev_has_event_code(dev, EV_KEY, BTN_LEFT) \
                || libevdev_has_event_code(dev, EV_KEY, BTN_TOUCH)){
                pthread_create(&thread_id[max_dev++], NULL, (void*) listen_device, (void *)dev);
                continue;
            }
        } else {
            break;
        }
    }
}

int client_main(){
    client_init();
    evdev_init();
    for(size_t i = 0;i<max_dev;i++){
        pthread_join(thread_id[i], NULL);
    }
    return 0;
}
