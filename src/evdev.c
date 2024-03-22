#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>

#include <pthread.h>
#include <string.h>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include <event.h>
#include <evdev.h>

static int fd = 1;
void evdev_init() {
    char devname[256] = "/dev/input/event";

    for (int i = 0; 1 ; ++i) {
        struct libevdev *dev = NULL;
        char devpath[256];
        sprintf(devpath, "%s%d", devname, i);
        fd = open(devpath, O_RDONLY);
        if (fd > 0) {
            libevdev_new_from_fd(fd, &dev);
            const char *name = libevdev_get_name(dev);
            if(strcmp(name, "31") == 0){
                continue;
            }
            if (libevdev_has_event_code(dev, EV_KEY, BTN_LEFT) \
                || libevdev_has_event_code(dev, EV_KEY, BTN_TOUCH)){
                pthread_t thread_id;
                pthread_create(&thread_id, NULL, (void * (*)(void *))listen_device, (void *)dev);
                continue;
            }
        } else {
            break;
        }
    }
}
