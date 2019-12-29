#ifndef STATIC_HEAP
#define STATIC_HEAP
#include <stdlib.h>
struct static_heap {
    void* malloced_ptr;
    size_t occupancy;
    size_t max;
};

struct static_heap make_heap(size_t s) {
    struct static_heap sh = {
        malloc(s),
        0,
        s
    };
    return sh;
}

void* sh_malloc(struct static_heap * sh, size_t amount) {
    assert((amount + sh->occupancy) <= sh->max);
    void* to_return = &(((byte *)sh->malloced_ptr)[sh->occupancy]);
    sh->occupancy += amount;
    return to_return;
}

void sh_free(struct static_heap * sh) {
    free(sh->malloced_ptr);
}

#endif // STATIC_HEAP
