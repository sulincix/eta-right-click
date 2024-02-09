#include <input.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <linux/uinput.h>

static int fd;

void uinput_init() {
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_EVBIT, EV_REL);

    struct uinput_user_dev uidev;
    memset(&uidev,0,sizeof(uidev));
    snprintf(uidev.name,UINPUT_MAX_NAME_SIZE,"EtaRightClick");
    uidev.id.bustype = BUS_USB;
    uidev.id.version = 1;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    sleep(2);
}


void do_right_click(){
    // TODO: write implementation
    puts("CLICK");
    // sleep 300ms
    usleep(300000);
    struct input_event ev_click, ev_sync;
    memset(&ev_click, 0, sizeof(ev_click));
    memset(&ev_sync, 0, sizeof(ev_sync));

    ev_click.type = EV_KEY;
    ev_click.code = BTN_RIGHT;
    ev_click.value = 1;

    //write right click event
    write(fd, &ev_click, sizeof(ev_click));

    //sync right click event
    ev_sync.type = EV_SYN;
    write(fd, &ev_sync, sizeof(ev_sync));

    // sleep 300ms
    usleep(300000);

    ev_click.type = EV_KEY;
    ev_click.code = BTN_RIGHT;
    ev_click.value = 0;

    //write right click event
    write(fd, &ev_click, sizeof(ev_click));

    //sync right click event
    ev_sync.type = EV_SYN;
    write(fd, &ev_sync, sizeof(ev_sync));

}
