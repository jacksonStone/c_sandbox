#ifndef GROWING_HEAP
#define GROWING_HEAP
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct {
    size_t occupancy;
    size_t max;
    void* malloced_ptr;
} growing_heap_bucket;

typedef struct {
    size_t bucket_occupancy;
    size_t bucket_max;
    size_t total_size;
    growing_heap_bucket* buckets_ptr;
    growing_heap_bucket* current_bucket;
} growing_heap;

growing_heap make_growing_heap(void) {
    growing_heap gh = {};
    return gh;
}
growing_heap make_growing_heap_with_size(size_t size) {
    
    growing_heap gh = {
        1,
        1,
        size
    };
    
    gh.buckets_ptr = malloc(sizeof(growing_heap_bucket) * 1);
    gh.current_bucket = gh.buckets_ptr;

    gh.buckets_ptr->occupancy = 0;
    gh.buckets_ptr->malloced_ptr = malloc(size);
    gh.buckets_ptr->max = size;
    
    return gh;
}
void* gh_malloc(growing_heap * gh, size_t amount);
void* gh_add_bucket_then_malloc(growing_heap * gh, size_t amount);

void* gh_malloc(growing_heap * gh, size_t amount) {
    // assert((amount + sh->occupancy) <= sh->max);
    if(gh->bucket_occupancy == 0) {
        //Create first bucket
        return gh_add_bucket_then_malloc(gh, amount);
    }
    //Otherwise we check if current bucket has what we need
    growing_heap_bucket current_bucket = *(gh->current_bucket);
    if (current_bucket.occupancy + amount > current_bucket.max) {
        // Need new bucket
        return gh_add_bucket_then_malloc(gh, amount);
    }
    void* to_return = &(((byte *)current_bucket.malloced_ptr)[current_bucket.occupancy]);
    gh->current_bucket->occupancy = amount + current_bucket.occupancy;
    return to_return;
}

void* gh_add_bucket_then_malloc(growing_heap * gh, size_t amount) {
    size_t bucket_max = gh->bucket_max;
    //Need to allocate bucket pointers
    if(bucket_max == 0) {
       gh->buckets_ptr = malloc(sizeof(growing_heap_bucket) * 2);
       gh->bucket_max = 2;
       return gh_add_bucket_then_malloc(gh, amount);
    }

    size_t next_bucket = gh->bucket_occupancy;
    if(next_bucket >= (bucket_max - 1)) {
        growing_heap_bucket* new_buckets = malloc(sizeof(growing_heap_bucket) * bucket_max * 2);
        memcpy(new_buckets, gh->buckets_ptr, bucket_max * sizeof(growing_heap_bucket));
        free(gh->buckets_ptr);
        gh->buckets_ptr = new_buckets;
        gh->bucket_max = bucket_max * 2;
        return gh_add_bucket_then_malloc(gh, amount);
    }
    //Create new bucket and insert into open slot
    size_t total_size = gh->total_size;
    size_t add_size = total_size;
    if(!add_size || (add_size < amount * 4)) {
        add_size = amount * 4;    
    }
    growing_heap_bucket new_bucket = {
        0,
        total_size,
        malloc(total_size)
    };
    (gh->buckets_ptr)[next_bucket] = new_bucket;
    gh->current_bucket = &((gh->buckets_ptr)[next_bucket]);
    gh->total_size = add_size + total_size;
    //TODO:: Verify this
    gh->bucket_occupancy += 1;
    return gh_malloc(gh, amount);
}

void gh_free(growing_heap * gh) {
    for(int i = 0; i < gh->bucket_occupancy; i++) {
        free(gh->buckets_ptr[i].malloced_ptr);
    }
    free((void *)(gh->buckets_ptr));
}

#endif // GROWING_HEAP
