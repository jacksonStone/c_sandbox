//Old bucket: 77 (.31)
//Straight malloc: 246 (1)
//One-time heap: 32 (.13)


#include <stdio.h>
#include <assert.h>
#include "../timer/timer.h"
#include "../dynamic_structures/list.h"
#include "../dynamic_structures/little_bucket.h"
typedef char byte;
#define kb 1024
#define mb kb*1024
#define gb mb*1024

struct tiny_heap {
    void* malloced_ptr;
    size_t occupancy;
    size_t max;
};

struct tiny_heap make_heap(size_t s) {
    struct tiny_heap th = {
        malloc(s),
        0,
        s
    };
    return th;
}

void* th_malloc(struct tiny_heap * th, size_t amount) {
    assert((amount + th->occupancy) <= th->max);
    void* to_return = &(((byte *)th->malloced_ptr)[th->occupancy]);
    th->occupancy += amount;
    return to_return;
}

void th_free(struct tiny_heap * th) {
    free(th->malloced_ptr);
}

void* individual_malloc(void) {
    return malloc(sizeof(void *));
}
void* my_malloc(struct tiny_heap * th) {
    return th_malloc(th, sizeof(void *));
}

void* old_bucket_malloc(struct little_bucket * bkt) {
    return alloc_bucket(bkt, sizeof(void *));
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
    struct tiny_heap th = make_heap(sizeof(foo));
    for(int i = 0; i < iterations; i++) {
        foo[i] = my_malloc(&th);
    }
    printf("Runtime for lump: %f µs\n", stop_timer());

    start_timer();
    struct little_bucket bkt = create_bucket_with_size(sizeof(foo));
    for(int i = 0; i < iterations; i++) {
        foo[i] = old_bucket_malloc(&bkt);
    }
    printf("Runtime for bucket: %f µs\n", stop_timer());


    return 0;
}

