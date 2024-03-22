#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdbool.h>

Display* display;
Window window;
XEvent e;



void init(){
    display = XOpenDisplay(NULL);
    window = XDefaultRootWindow(display);

}

void block(){
    unsigned int masks = PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
    XGrabPointer(display, window, True, PointerMotionMask | ButtonPressMask | ButtonReleaseMask ,
        GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
}

void unblock(){
    XUngrabPointer(display, CurrentTime);
}

int main(int argc, char const *argv[])
{
    init();
    block();

    while(1){
        if (XPending(display))
            XNextEvent(display, &e);
    }

    XCloseDisplay(display);

    return 0;
}