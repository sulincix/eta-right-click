#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdbool.h>


#include <socket.h>

Display* display;
Window window;
XEvent e;



void init(){
    display = XOpenDisplay(NULL);
    window = XDefaultRootWindow(display);

}

void block(){
    unsigned int masks = PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
    XGrabPointer(display, window, True, masks ,
        GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
}

void unblock(){
    XUngrabPointer(display, CurrentTime);
}

int main(int argc, char const *argv[])
{
    socket_init();
    init();
    block();

    while(1){
        char* msg;
        msg = socket_read();
        printf("%c\n", msg[0]);
    }

    XCloseDisplay(display);

    return 0;
}
