#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <epoch_time.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>


#include <debug.h>

void listen_device(struct libevdev *dev) {
    const char *name = libevdev_get_name(dev);
    struct libevdev_uinput *uidev = NULL;
    debug("%s \n", name);
    bool lock = false;
    int rc = 0;
    libevdev_set_name(dev, "31");
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_MIDDLE, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOUCH, NULL);

    rc = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    ioctl(libevdev_get_fd(dev), EVIOCGRAB, 1);
    int mouse_moved = 0;
    int pressed = 0; // To track whether a key is pressed
    int64_t press_start; // To track the start time of key press
    int64_t press_end;
    int64_t press_duration;
    int press_key = BTN_LEFT;

    int saved_x, saved_y;

    while (1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if(rc != 0) {
            continue;
        }
        if(lock){
            continue;
        }
        lock = true;
        if (ev.type == EV_KEY){
            if(ev.code == BTN_LEFT || ev.code == BTN_TOUCH){
                if(ev.value == 1){ // press event
                    pressed = 1;
                    press_start = epoch();
                    //press_key = ev.code;
                } else {
                    press_end = epoch();
                    press_duration = (int)(press_end - press_start);
                    mouse_moved = 0;
                    if(mouse_moved == 0){
                        debug("MOVE DONE\n");
                        libevdev_uinput_write_event(uidev, EV_KEY, press_key, 0);
                        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                        usleep(10000);
                    } else if(press_duration > 300) { // righ click
                        debug("RIGHT CLICK %d\n", press_duration);
                        libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 1);
                        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                        usleep(10000);
                        libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 0);
                        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                    } else { // left click
                        debug("LEFT CLICK %d\n", press_duration);
                        libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 1);
                        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                        usleep(10000);
                        libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 0);
                        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                    }
                }
            }
        } else if (ev.type == EV_ABS){
            if(epoch() - press_start > 100) { // move event
                if (pressed && mouse_moved == 0) {
                    mouse_moved = 1;
                    libevdev_uinput_write_event(uidev, EV_KEY, press_key, 1);
                    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
                    usleep(10000); // Sleep for 10ms
                    printf("AMOGUS:%d\n", epoch() - press_start);
                }
                // TODO; add event restore
                libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
                printf("MOVE:%d\n", epoch() - press_start);
            }else {
                // TODO: add event save
                printf("BLOCK:%d\n", epoch() - press_start);
            }

        } else { // Non-key event when a key is pressed
            rc = libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
        }
        lock = false;
    }
}

