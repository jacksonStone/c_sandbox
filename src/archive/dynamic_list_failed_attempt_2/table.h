// A string indexed table
typedef int (*key_hasher)(int hash_range, char* key);

struct {
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
    void ** contents;
    char ** keys;
    int entry_count;
} typedef hash_table;

hash_table make_table(int starting_hash_range, float rehash_threshold);
void put_entry(hash_table * table, char * key, void * content);
void * get_entry(hash_table table, char * key);