#ifndef JS_HASHMAP
#define JS_HASHMAP
#include "./growing_heap.h"
#include "./list.h"
#include "./types.h"

typedef struct {
    uint hash;
    char * key;
    size_t key_len;
    void * value;
} hashmap_entry;
typedef struct {
    int can_free;
    growing_heap* gh;
    hashmap_entry* entries;
    int entry_capacity;
    int entry_count;
} hashmap;

uint hash_string(char * key, size_t len) {
    uint hash = 0;
    while (len--)
        hash = (hash << 7) + (hash >> 25) + *key++;
    return (hash + (hash >> 16));
}

// When passing in an allocator right now
// that means it will not free slots on list resizing 
// because gh does not support partial frees right now.
hashmap make_hashmap(growing_heap * gh){
    hashmap hm = {
        0,
        gh
    };
    return hm;
}

void attach_gh_to_hm(hashmap* hm) {
    growing_heap* gh_ptr = malloc(sizeof(growing_heap));
    *gh_ptr = make_growing_heap();
    hm->gh = gh_ptr;
}
hashmap make_hashmap_no_allocator() {
    hashmap hm = {
        1
    };
    attach_gh_to_hm(&hm);
    return hm;
}

void release_hashmap_memory(hashmap* hm) {
    gh_free(hm->gh);
    free(hm->gh); // since we malloced this
}
void reset_hashmap_memory(hashmap* hm) {
   if(hm->can_free) {
        release_hashmap_memory(hm);
        attach_gh_to_hm(hm);
    } 
}

#define EMPTY_HASH 0
#define REMOVED_HASH 1
#define FIRST_VALID_HASH 2
#define MINIMUM_MAP_SPACE 32 // 32 spaces by default. Because that seems like a good number.

void add_to_hashmap(hashmap * hm, char* key, size_t key_len, void* entry) {
    if(hm->entry_count == 0) {
        hm->entries = gh_malloc(hm->gh, sizeof(hashmap_entry)*MINIMUM_MAP_SPACE); 
        hm->entry_capacity = MINIMUM_MAP_SPACE;
        for(int i = 0; i < MINIMUM_MAP_SPACE; i++) {
            hm->entries[i].hash = EMPTY_HASH; //Mark them all as empty
        }
    } else if(hm->entry_capacity * 7 < hm->entry_count * 10) {
        //At .7 saturation we resize
        uint old_capacity = hm->entry_capacity;
        hashmap_entry old_entries[old_capacity];
        memcpy(old_entries, hm->entries, sizeof(hashmap_entry) * old_capacity);
        reset_hashmap_memory(hm);
        hm->entry_capacity *= 2;
        hm->entries = gh_malloc(hm->gh, sizeof(void*)*hm->entry_capacity);
        hm->entry_count = 0;

        for(int i = 0; i < hm->entry_capacity; i++) {
            hm->entries[i].hash = EMPTY_HASH;
        }

        for(int i = 0; i < old_capacity; i++) {
            if(old_entries[i].hash >= FIRST_VALID_HASH) {
                //Hash old entry into the new list
                add_to_hashmap(hm, old_entries[i].key, old_entries[i].key_len, old_entries[i].value);
            }
        }
    }
    
    uint hash_result = hash_string(key, key_len);
    if(hash_result < FIRST_VALID_HASH) {
        hash_result += FIRST_VALID_HASH;
    }
    uint entry_index = hash_result % (hm->entry_capacity);

    while(hm->entries[entry_index].hash > FIRST_VALID_HASH) {
        hashmap_entry hme = hm->entries[entry_index];
        if(hme.hash == hash_result && hme.key_len == key_len && strncmp(hme.key, key, key_len) == 0) {
            //Replacing existing hashmap entry
            hme.value = entry;
            return;
        }
        entry_index = (entry_index + 1) % hm->entry_capacity;
    }
    //Is a new entry and entry_index is an open slot in the hm
    hashmap_entry new_hme = {
        hash_result,
        key,
        key_len,
        entry
    };
    hm->entries[entry_index] = new_hme;
    hm->entry_count += 1;
}

void* get_hashmap_entry(hashmap* hm, char* key, int len) {
    uint hash_result = hash_string(key, len);
    uint entry_index = hash_result % hm->entry_capacity;
    while(hm->entries[entry_index].hash > FIRST_VALID_HASH) {
        hashmap_entry hme = hm->entries[entry_index];
        if(hme.hash == hash_result && hme.key_len == len && strncmp(hme.key, key, len) == 0) {
            //Replacing existing hashmap entry
            return hme.value;
        }
        entry_index = (entry_index + 1) % hm->entry_capacity;
    }
    //NOT FOUND
    return NULL;
}

void add_to_hashmap_c_string(hashmap * hm, char* key, void* entry) {
    add_to_hashmap(hm, key, strlen(key), entry);
};
void* get_hashmap_entry_c_string(hashmap* hm, char* key) {
    return get_hashmap_entry(hm, key, strlen(key));
}
#endif