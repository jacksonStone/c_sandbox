#include "./list.h"
#include "./hash_map.h"
#include <string.h>
#include <assert.h>
struct {
    char * key;
    void * content;
} typedef hash_entry;

#define STARTING_BUCKET_COUNT 4
#define STARTING_BUCKET_SIZE 1

hash_map _make_hash_map(size_t sizeOfEntry) {
    hash_map map;
    map.entry_count = 0;
    map.entry_keys = make_list(char *);
    map.buckets = make_list_with_starting_length(void *, STARTING_BUCKET_COUNT);
    for(int i = 0; i < STARTING_BUCKET_COUNT; i++) {
        map.buckets = append_to_list(map.buckets, make_list_with_starting_length(hash_entry, STARTING_BUCKET_SIZE));
    }
    map.bucket_count = STARTING_BUCKET_COUNT;
    return map;
}
unsigned int hash_string(char * key, unsigned int range) {
    unsigned int hash = 0;
    while(*key) {
        hash = (hash << 7) + (hash >> 25) + *key;
        key++;
    }
    return hash % range;
}
void _add_hash_entry(hash_map* map, char * key, void * content) {

    assert(list_entry_count(map->buckets) == map->bucket_count);
    
    int bucket_index = hash_string(key, map->bucket_count);
    hash_entry * bucket = map->buckets[bucket_index];
    for(int i = 0; i < list_entry_count(bucket); i++) {
        if(strcmp(bucket[i].key, key) == 0) {
            bucket[i].content = content;
            return;
        }
    }
    
    //Create new entry
    hash_entry new_entry;
    new_entry.content = content;
    new_entry.key = key;
    
    //Make updates to map
    map->entry_count++;
    map->entry_keys = append_to_list(map->entry_keys, key);
    map->buckets[bucket_index] = append_to_list(bucket, new_entry);

    if(map->entry_count > (map->bucket_count * 4)) {
        //Rehash everything
        //Grab a reference to everything
        int entry_count = map->entry_count;
        hash_entry all_entries[entry_count];
        int entry_index = 0;
        for(int i = 0; i < list_entry_count(map->buckets); i++) {
            hash_entry * cur_bucket = map->buckets[i];
            for(int j = 0; j < list_entry_count(cur_bucket); j++) {
                hash_entry entry_copy;
                entry_copy.key = cur_bucket[j].key;
                entry_copy.content = cur_bucket[j].content;
                all_entries[entry_index] = entry_copy;
                entry_index++;
            }
            free_list(map->buckets[i]);
        }
        //Clear out keys
        free_list(map->entry_keys);
        map->entry_keys = make_list_with_starting_length(char *, entry_count * 2);

        //Create new buckets
        free_list(map->buckets);
        map->bucket_count *= 2;
        map->buckets = make_list_with_starting_length(void *, map->bucket_count);
        for(int i = 0; i < map->bucket_count; i++) {
            map->buckets = append_to_list(map->buckets, make_list_with_starting_length(hash_entry, STARTING_BUCKET_SIZE));
        }
        map->entry_count = 0;

        //Rehash
        for(int i = 0; i < entry_count; i++) {
            _add_hash_entry(map, all_entries[i].key, all_entries[i].content);
        }
    }
}
void * get_hash_entry(hash_map map, char * key) {
    hash_entry * bucket = map.buckets[hash_string(key, map.bucket_count)];
    int bucket_len = list_entry_count(bucket);
    for(int i = 0; i < bucket_len; i++) {
        if(strcmp(bucket[i].key, key) == 0) {
            return bucket[i].content;
        }
    }
    return NULL;
}