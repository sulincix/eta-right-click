#include <event.h>
#include <stdlib.h>
#include <debug.h>

#include <input.h>

#include <epoch_time.h>

void listen_device(struct libevdev *dev){
    const char *name = libevdev_get_name(dev);
    printf("%s \n",name);
    int64_t t = epoch();
    int rc = 0;
    while(1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0) {
            if (ev.type <= 0){
                continue;
            }
            if (ev.type == EV_KEY || ev.code == BTN_TOUCH) {
                if (ev.value == 1) {
                    debug("press %ld\n", epoch() - t);
                    t = epoch();
                } else if (epoch() - t > 700) {
                    debug("release %ld\n", epoch() - t);
                    do_right_click();
                }
//                debug("Event type: %ld, code: %d, value: %d\n", ev.type, ev.code, ev.value);

            } else if (ev.type == EV_ABS || ev.type == EV_REL) {
                if(epoch() - t > 100){
                    continue;
                }
                debug("move %ld\n", epoch() - t);
                t = epoch();

//                debug("Event type: %ld, code: %d, value: %d\n", ev.type, ev.code, ev.value);
            }
        } else if (rc != LIBEVDEV_READ_STATUS_SYNC) {
            fprintf(stderr, "Error reading events: %d\n", rc);
        }
   }
}
