#include "./list.h"
// A string indexed table
typedef int (*key_hasher)(int hash_range, char* key);
typedef struct {
    char * key;
    void * contents;
} hash_entry;

typedef struct {
    // Number of buckets in the hash_table
    int hash_range;
    // If you wish to specify a custom hashing function
    key_hasher hashing_function;
    // When the total number of entries exceeds this 
    // ratio of entries/hash_range
    // The hashtable will expand and re-hash.
    // If 0, it will not rehash automatically.
    float rehash_threshold;
    // A pointer to the table buckets
    hash_entry ** contents;
    int entry_count;
} hash_table;
int left_rotate_bits(int num, int rotation) {
    return (num << rotation)|(num >> ((int)sizeof(num) - rotation));
}
int default_hash_function(int hash_range, char* key) {
    int result = 0x55555555;

    while (*key) { 
        result += (int) *key++;
        result = left_rotate_bits(result, 1);
    }
    int modded = result % hash_range;
    while(modded < 0) {
        modded += hash_range;
    }
    return modded;
}
hash_table _make_table(int starting_hash_range, float rehash_threshold) {
    hash_table new_table;
    new_table.hashing_function = default_hash_function;
    new_table.hash_range = starting_hash_range;
    new_table.rehash_threshold = rehash_threshold;
    new_table.contents = make_list_with_initial_capacity(hash_entry, starting_hash_range);
    new_table.entry_count = 0;
    return new_table;
}
void _put_entry(hash_table table, char * key, void * content) {
    int hash = table.hashing_function(table.hash_range, key);
}
void * _get_entry(char * key) {
    void * null; 
    return null;
}
