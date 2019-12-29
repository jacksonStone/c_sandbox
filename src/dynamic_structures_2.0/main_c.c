#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../timer/timer.h"
#include "./types.h"
#include "./static_heap.h"
void* individual_malloc(void) {
    return malloc(sizeof(void *));
}
void* my_malloc(struct static_heap * sh) {
    return sh_malloc(sh, sizeof(void *));
}

int main() {
    int iterations = 5000;
    void* foo[iterations];
    start_timer();

    for(int i = 0; i < iterations; i++) {
        foo[i] = individual_malloc();
    }
    printf("Runtime for individual: %f µs\n", stop_timer());

    start_timer();
    struct static_heap sh = make_heap(sizeof(foo));
    for(int i = 0; i < iterations; i++) {
        foo[i] = my_malloc(&sh);
    }
    printf("Runtime for lump: %f µs\n", stop_timer());

    return 0;
}

