#include <string.h>
extern int server_main();
extern int client_main();

int main(int argc, char** argv){
    if(argc > 1){
        if(strcmp(argv[1],"server") == 0){
            return server_main();
        }
    }
    return client_main();
}