#include <evdev.h>
#include <input.h>

int main(){
    evdev_init();
    uinput_init();
    while(1){
        // infinitive wait for threads
        sleep(1);
    }
    return 1;
}
