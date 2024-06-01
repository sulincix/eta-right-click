#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdint.h>

#include "socket.h"

#define die(str, args...) { \
	perror(str); \
	exit(EXIT_FAILURE); \
}

int rc;
struct libevdev *dev = NULL;
struct libevdev_uinput *uidev;
struct input_event ev;

int server_main() {

    socket_init();

    dev = libevdev_new();

    // replace vendor info
    libevdev_set_name(dev, "Amogus mouse");
    libevdev_set_id_bustype(dev, BUS_USB);
    libevdev_set_id_vendor(dev, 0x1453);
    libevdev_set_id_product(dev, 0x1299);
    libevdev_set_id_version(dev, 0x1);

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
    libevdev_enable_event_code(dev, EV_ABS, ABS_PRESSURE, NULL);


    // Enable touch-related event codes for a virtual touch screen
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_SLOT, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TOUCH_MAJOR, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TOUCH_MINOR, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_X, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_Y, NULL);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_TRACKING_ID, NULL);

    // set axis ranges and resolution
    struct input_absinfo abs_info_x, abs_info_y, abs_info_pressure;
    abs_info_x.minimum = 0;
    abs_info_x.maximum = UINT16_MAX;
    abs_info_x.fuzz = 0;
    abs_info_x.flat = 0;
    abs_info_x.resolution = 400;
    libevdev_set_abs_info(dev, ABS_X, &abs_info_x);

    abs_info_y.minimum = 0;
    abs_info_y.maximum = UINT16_MAX;
    abs_info_y.fuzz = 0;
    abs_info_y.flat = 0;
    abs_info_y.resolution = 400;
    libevdev_set_abs_info(dev, ABS_Y, &abs_info_y);

    abs_info_pressure.minimum = 0;
    abs_info_pressure.maximum = INT16_MAX;
    abs_info_pressure.fuzz = 0;
    abs_info_pressure.flat = 0;
    abs_info_pressure.resolution = 0;
    libevdev_set_abs_info(dev, ABS_PRESSURE, &abs_info_pressure);

    // Initialize uinput device
    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);

    rc = libevdev_uinput_write_event(uidev, EV_ABS, ABS_Y, 0);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);

    if (rc < 0) {
        fprintf(stderr, "Failed to create uinput device: %s\n", strerror(rc));
        libevdev_free(dev);
        return -1;
    }

	// sleep 300ms
    usleep(300);

//    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);


    while (1) {
        socket_data data = socket_read();

        if (data.action == UPDATE){
            //update_abs(data.abs_info_x, data.abs_info_y);
            continue;
        }

        ev.type = data.ev_type;
        ev.code =  data.ev_code;
        ev.value = data.ev_value;
        printf("Event: %d %d %d\n",
            ev.type,
            ev.code,
            ev.value
        );
        rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        memset(&ev, 0, sizeof(ev));

    }

    (void)rc;
    libevdev_free(dev);
    return 0;
}