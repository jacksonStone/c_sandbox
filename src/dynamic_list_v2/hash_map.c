#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "./dynamic_list.c"
struct {
    char * key;
    void * content;
} typedef hash_entry;

struct {
    void ** buckets;
    char ** entry_keys;
    int bucket_count;
    int entry_count;
} typedef hash_map;



#define STARTING_BUCKET_COUNT 1

#define make_hash_map(type) _make_hash_map(sizeof(type))

hash_map _make_hash_map(size_t sizeOfEntry) {
    hash_map map;
    map.entry_count = 0;
    map.entry_keys = make_list(char *);
    map.buckets = make_list_with_starting_length(void *, STARTING_BUCKET_COUNT);
    for(int i = 0; i < STARTING_BUCKET_COUNT; i++) {
        map.buckets = append_to_list(map.buckets, make_list_with_starting_length(hash_entry, 1));
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
#define add_hash_entry(map, key, content) _add_hash_entry(&map, key, content);
void _add_hash_entry(hash_map* map, char * key, void * content) {

    assert(list_entry_count(map->buckets) == map->bucket_count);
    
    for(int i = 0; i < list_entry_count(map->buckets); i++) {
        assert(list_already_freed(map->buckets[i]) == 0);
    }

    int bucket_index = hash_string(key, map->bucket_count);
    hash_entry * bucket = map->buckets[bucket_index];
    for(int i = 0; i < list_entry_count(bucket); i++) {
        if(strcmp(bucket[i].key, key) == 0) {
            printf("matching key: %s!\n", key);
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
    printf("bucket index %d\n", bucket_index);

    if(map->entry_count > (map->bucket_count * 3)) {
        //Rehash everything
        //Grab a reference to everything
        int entry_count = map->entry_count;
        hash_entry all_entries[entry_count];
        int entry_index = 0;
        for(int i = 0; i < list_entry_count(map->buckets); i++) {
            hash_entry * cur_bucket = map->buckets[i];
            assert(list_already_freed(map->buckets[i]) == 0);
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
        assert(!list_already_freed(map->buckets));
        free_list(map->buckets);
        map->bucket_count *= 2;
        map->buckets = make_list_with_starting_length(void *, map->bucket_count);
        for(int i = 0; i < map->bucket_count; i++) {
            map->buckets = append_to_list(map->buckets, make_list_with_starting_length(hash_entry, 1));
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
    for(int i = 0; i < list_entry_count(bucket); i++) {
        if(strcmp(bucket[i].key, key) == 0) {
            return bucket[i].content;
        }
    }
    return NULL;
}

//TODO REMOVE
#include <stdio.h>
int main() {
    hash_map map = make_hash_map(char *);
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    int count = 10000;
    char keys[count][6];
    char contents[count][6];
    for(int i = 0; i < count; i++) {
        sprintf(keys[i], "%dfoo%d", i, i);
        sprintf(contents[i], "%dHello%d", i, i);
        // printf("Key: %s, content: %s\n", keys[i], contents[i]);
        add_hash_entry(map, keys[i], contents[i]);
    }
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    return 0;
}

