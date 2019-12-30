#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../timer/timer.h"
#include "./types.h"
#include "./static_heap.h"
#include "./growing_heap.h"
#include "./list.h"

typedef struct {
    char * key;
    int key_len;
    void * value;
} hashmap_value;
typedef struct {
    hashmap_value * values;
} hashmap_bucket;
typedef struct {
    growing_heap* gh;
    hashmap_bucket* buckets;
    int entry_count;
} hashmap;

unsigned int hash_string(char * key, size_t len, unsigned int range) {
    unsigned int hash = 0;
    while(len--) {
        hash = (hash << 7) + (hash >> 25) + *key;
        key++;
    }
    return hash % range;
}

hashmap make_hashmap(growing_heap * gh){
    hashmap hm = {
        gh
    };
    return hm;
}

void add_to_hashmap(hashmap * hm, char* key, size_t key_len, void* entry) {
    if(hm->entry_count == 0) {
        //Allot everything
        int bucket_count = 4;
        hm->buckets = make_list_with_allocator_with_size(hashmap_bucket, bucket_count, hm->gh);
        for(int i = 0; i < 4; i++) {
            hashmap_bucket bucket = {};
            bucket.values = make_list_with_allocator_with_size(hashmap_value, 2, hm->gh);
            hm->buckets = append_to_list(hm->buckets, bucket);
        }

        unsigned int bucket_index = hash_string(key, key_len, bucket_count);
        hashmap_bucket* target_bucket = (hm->buckets + bucket_index);
        hashmap_value hv = {
            key, 
            key_len,
            entry
        };
        target_bucket->values = append_to_list(target_bucket->values, hv);
        hm->entry_count = 1;
        return;
    }
    int bucket_count = get_list_len(hm->buckets);
    if(hm->entry_count > bucket_count * 8) {
        //Resize
        assert(0);
    }

    unsigned int bucket_index = hash_string(key, key_len, bucket_count);
    hashmap_bucket* target_bucket = (hm->buckets + bucket_index);
    int bucket_entries = get_list_len(target_bucket->values);
    if(bucket_entries == 0) {
        hashmap_value hv = {
            key, 
            key_len,
            entry
        };
        //We can append - no need to loop through entries
        target_bucket->values = append_to_list(target_bucket->values, hv);
    }
    for(int i = 0; i < bucket_entries; i++) {
        hashmap_value hv = target_bucket->values[i];
        if(hv.key_len == key_len) {
            if(memcmp(hv.key, key, key_len) == 0) {
                (target_bucket->values + i)->value = entry;
                return;
            }
        }
    }
    hashmap_value hv = {
        key,
        key_len,
        entry
    };
    target_bucket->values = append_to_list(target_bucket->values, hv);
}

void add_to_hashmap_c_string(hashmap * hm, char* key, void* entry) {
    add_to_hashmap(hm, key, strlen(key), entry);
};


int main() {
    int allot_size = 500;
    start_timer();
    growing_heap gh = make_growing_heap_with_size(allot_size*32);
    char* my_str = gh_malloc(&gh, allot_size);
    int* my_ints = make_list_with_allocator(int, &gh);
    my_ints = append_to_list(my_ints, 4);
    hashmap hm = make_hashmap(&gh);
    int k = 4;
    add_to_hashmap_c_string(&hm, "abc", &k);
    //Is a no-op if you pass an allocator during list creation
    free_list(my_ints);
    gh_free(&gh);
    printf("time: %f microsec\n", stop_timer());
    return 0;
}

