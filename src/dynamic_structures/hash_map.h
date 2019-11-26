#include <stdlib.h>
struct {
    void ** buckets;
    char ** entry_keys;
    int bucket_count;
    int entry_count;
} typedef hash_map;

#define make_hash_map(type) _make_hash_map(sizeof(type))
hash_map _make_hash_map(size_t sizeOfEntry);

#define add_hash_entry(map, key, content) _add_hash_entry(&map, key, content)
void _add_hash_entry(hash_map* map, char * key, void * content);

void * get_hash_entry(hash_map map, char * key);