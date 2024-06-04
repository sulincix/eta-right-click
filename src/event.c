#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <linux/uinput.h>
#include <epoch_time.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include <debug.h>

void listen_device(struct libevdev *dev) {
    const char *name = libevdev_get_name(dev);
    debug("%s \n", name);
    int rc = 0;
    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);
    while (1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        printf("%d %d %d\n", ev.type, ev.code, ev.value);
        //rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
    }
}

