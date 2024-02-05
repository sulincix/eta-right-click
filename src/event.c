#include <event.h>
#include <stdlib.h>
#include <debug.h>

void listen_device(struct libevdev *dev){
    const char *name = libevdev_get_name(dev);
    printf("%s \n",name);
    int rc = 0;
    while(1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0) {
            if (ev.type == EV_KEY || ev.code == BTN_TOUCH) {
                debug("Event type: %d, code: %d, value: %d\n", ev.type, ev.code, ev.value);
            
            } else if (ev.type == EV_ABS || ev.type == EV_REL) {
                debug("Event type: %d, code: %d, value: %d\n", ev.type, ev.code, ev.value);
            }
        } else if (rc != LIBEVDEV_READ_STATUS_SYNC) {
            fprintf(stderr, "Error reading events: %d\n", rc);
        }
   }
}
