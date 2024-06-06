#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

struct timeout_ctx {
    int delay;
    void* args;
    int id;
    void (*function)(void*);
};

int action_id = 0;

void* timeout_worker(struct timeout_ctx *ctx){
    usleep(ctx->delay*1000);
    if(ctx->id == action_id){
        ctx->function(ctx->args);
    }
    return NULL;
}

void timeout_add(int timeout, void* function, void* args){
    pthread_t thread_id;
    struct timeout_ctx *ctx = calloc(1, sizeof(struct timeout_ctx));
    ctx->delay = timeout;
    ctx->id = action_id;
    ctx->function = function;
    ctx->args = args;
    pthread_create(&thread_id, NULL, (void*)timeout_worker, (void*)ctx);
}

#include <sys/time.h>

uint64_t micro_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
}
