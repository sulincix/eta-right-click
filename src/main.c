#include <evdev.h>
#include <input.h>
#include <socket.h>
#include <unistd.h>

int main(){
    send_all("a");
    evdev_init();
    uinput_init();
    while(1){
        // infinitive wait for threads
        sleep(1);
    }
    return 1;
}
