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
    const char *name = libevdev_get_name(dev);
    int vendor = libevdev_get_id_vendor(dev);
    if(vendor == 0x1453){
        puts("amogus");
        return;
    }
    printf("%s\n", name);
    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);
    printf("%d %d\n", libevdev_get_abs_maximum(dev, ABS_Y), libevdev_get_abs_minimum(dev, ABS_Y));
    while (1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        printf("%d %d %d\n", ev.type, ev.code, ev.value);
        int data[SOCKET_BUFFER_LENGTH];
        data[0] = ev.type;
        data[1] = ev.code;
        data[2] = ev.value;
        client_send(data);
        //rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
    }
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
            const char *name = libevdev_get_name(dev);
            if(strcmp(name, "31") == 0){
                continue;
            }
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
