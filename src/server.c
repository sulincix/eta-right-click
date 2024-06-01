#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#include "socket.h"


int server_main() {

    socket_init();

    int rc;
    struct libevdev *dev = NULL;
    struct libevdev_uinput *uidev;
    struct input_event ev;

    dev = libevdev_new();
    // replace vendor info
    libevdev_set_name(dev, "Amogus mouse");
    libevdev_set_id_vendor(dev, 0x1453);
    libevdev_set_id_product(dev, 0x1299);

    // Enable event types
    libevdev_enable_event_type(dev, EV_SYN);
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_type(dev, EV_REL);
    libevdev_enable_event_type(dev, EV_ABS);

    // Enable key event codes for a virtual touch screen
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOUCH, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOOL_FINGER, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOOL_PEN, NULL);

    // Enable mouse event codes for a virtual mouse
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_MIDDLE, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_Y, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL, NULL);

    // Enable touch retared event codes for single finger touch screen
    libevdev_enable_event_code(dev, EV_ABS, ABS_X, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_Y, NULL);

    // Enable touch-related event codes for a virtual touch screen
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_SLOT, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TOUCH_MAJOR, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TOUCH_MINOR, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_X, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_Y, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TRACKING_ID, NULL);


    // enable altgr
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, KEY_RIGHTALT, NULL);

    // Initialize uinput device
    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (rc < 0) {
        fprintf(stderr, "Failed to create uinput device: %s\n", strerror(rc));
        libevdev_free(dev);
        return -1;
    }

	// sleep 300ms
    usleep(300);

//    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);


    while (1) {
        int *data = socket_read();
        ev.type = (int)data[0];
        ev.code = (int)data[1];
        ev.value = (int)data[2];
        printf("Event: %d %d %d\n",
            ev.type,
            ev.code,
            ev.value);
        rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    (void)rc;
    libevdev_free(dev);

    return 0;
}