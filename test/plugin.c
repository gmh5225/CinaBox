#! -I../libs/
#include "stdbool.h"
#include "stdio.h"
#include "empty.cpp"

bool stop_requested(struct stop_token* stop);



void load(void) {
    printf("loaded\n");
}

void start(void) {
    printf("started\n");
}

void go(struct stop_token* stop) {
    printf("gone\n");

    while(!stop_requested(stop)) {
        // printf("in thread\n");
    }
}

// int step(void) {
//     printf("step\n");
//     return 0;
// }

// void stop(void) {
//     printf("stopped!");
// }