#ifndef _event_h
#define _event_h

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <stdio.h>

void listen_device(struct libevdev *dev);
#endif
