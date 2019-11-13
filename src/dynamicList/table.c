#include "./list.h"
#include <stdio.h>
#include <string.h>
#include "./table.h"

struct {
    char * key;
    void * contents;
} typedef hash_entry;

int default_hash_function(int hash_range, char* key) {
    int result = 0xFFFFFF;
    while (*key) { 
        result += *key++;
    }
    int modded = result % hash_range;
    while(modded < 0) {
        modded += hash_range;
    }
    return modded;
}
hash_table make_table(int starting_hash_range, float rehash_threshold) {
    hash_table new_table;
    new_table.hashing_function = default_hash_function;
    new_table.hash_range = starting_hash_range;
    new_table.rehash_threshold = rehash_threshold;
    void * contentPointers = make_list_with_initial_capacity(void *, starting_hash_range);
    for(int i = 0; i < starting_hash_range; i++) {
        void * new_bucket = make_list_with_initial_capacity(hash_entry, 8);
        append(contentPointers, new_bucket);
    }
    new_table.contents = contentPointers;
    int starting_range = starting_hash_range * rehash_threshold;
    new_table.keys = make_list_with_initial_capacity(char *, starting_range);
    new_table.entry_count = 0;
    return new_table;
}
void put_entry(hash_table * table_ptr, char * key, void * content) {
    hash_table table = *table_ptr;
    int hash = table.hashing_function(table.hash_range, key);
    char ** keys = table.keys;
    hash_entry new_entry;
    new_entry.contents = content;
    new_entry.key = key;
    hash_entry * bucket = table.contents[hash];
    printf("HASH: %d\n", hash);
    printf("key: %s\n", key);
    //BREAKS AT GET LENGTH
    int current_length = get_list_length(bucket);
    printf("length: %d\n", current_length);

    for(int i = 0; i < current_length; i++) {
        printf("In loop\n");
        if(strcmp(key, bucket[i].key) == 0) {
            bucket[i].contents = content;
            return;
        }
    }
        printf("HERE\n");
    float ratio_of_entries_to_buckets = ((float)(table.entry_count + 1))/((float)table.hash_range);
    printf("HERE\n");
    // printf("ratio_of_entries_to_buckets %f\n", ratio_of_entries_to_buckets);
    while(table.rehash_threshold < ratio_of_entries_to_buckets) {
        printf("Rehashing!!\n");
        // Grab all current entries
        hash_entry all_entries[table.entry_count];
        for(int i = 0; i < table.entry_count; i++) {
            hash_entry current_hash_entry;
            current_hash_entry.contents = get_entry(table, keys[i]);
            current_hash_entry.key = keys[i];
            all_entries[i] = current_hash_entry;
        }
        // Create new buckets
        int old_range = table.hash_range;
        int new_range = table.hash_range * 2;
        char ** keys = table.keys;
        for(int i = 0; i < old_range; i++) {
            set_list_length(table.contents[i], 0);
        }
        //Need to add free list, and then create a new list of lists
        for(int i = old_range; i < new_range; i++) {
            void * new_bucket = make_list_with_initial_capacity(hash_entry, 8);
            append(table.contents, new_bucket);
        }
        printf("LL: %d\n", get_list_length(table.contents));
        int entry_count = table.entry_count;
        (*table_ptr).contents = table.contents;
        // update range
        (*table_ptr).entry_count = 0;
        (*table_ptr).hash_range = new_range;
        printf("New Range: %d\n", new_range);
        // Rehash everything
        for(int i = 0; i < entry_count; i++) {
            hash_entry cur = all_entries[i];
            printf("About to call put\n");
            put_entry(table_ptr, cur.key, cur.contents);
            printf("called put\n");
        }
        printf("Rehashed everything!");

    }
    (*table_ptr).entry_count += 1;
    printf("Entry count: %d\n", table.entry_count);
    append(bucket, new_entry);
    append(keys, key);
}
void * get_entry(hash_table table, char * key) {
    int hash = table.hashing_function(table.hash_range, key);
    hash_entry * bucket = table.contents[hash];
    int current_length = get_list_length(bucket);
    printf("LENGTH: %d\n", current_length);
    for(int i = 0; i < current_length; i++) {
        if(strcmp(key, (bucket[i]).key) == 0) {
            return (bucket[i]).contents;
        }
    }
    return NULL;
}
