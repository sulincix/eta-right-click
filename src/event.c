#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <epoch_time.h>

#include <string.h>

#define block_press() ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);
#define unblock_press() ioctl(libevdev_get_fd(dev), EVIOCGRAB, 0);

void handle_click(struct libevdev_uinput *uidev, int button) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));

    // Set event type and button code
    ev.type = EV_KEY;
    ev.code = button;
    ev.value = 1; // Press down
    libevdev_uinput_write_event(uidev, EV_KEY, button, 1);
    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    usleep(10000); // Sleep for 10ms
    libevdev_uinput_write_event(uidev, EV_KEY, button, 0); // Release
    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0); // Sync
}

void listen_device(struct libevdev *dev) {
    const char *name = libevdev_get_name(dev);
    struct libevdev_uinput *uidev = NULL;
    printf("%s \n", name);
    int64_t t = epoch();
    int rc = 0;
    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    block_press();
    int mouse_moved = 0;
    while (1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
    }
}


