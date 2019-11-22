#include "./hash_map.c"
#include <stdio.h>
int main() {
    hash_map map = make_hash_map(char *);
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    int count = 10000;
    char keys[count][20];
    char contents[count][20];
    for(int i = 0; i < count; i++) {
        sprintf(keys[i], "%dfoo%d", i, i);
        sprintf(contents[i], "%dHello%d", i, i);
        // printf("Key: %s, content: %s\n", keys[i], contents[i]);
        add_hash_entry(map, keys[i], contents[i]);
        add_hash_entry(map, keys[i/2], contents[i/2]);
    }
    for(int i = 0; i < count; i++) {
        assert(strcmp(get_hash_entry(map, keys[i]), contents[i]) == 0);
    }
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    char ** char_list = make_list(char *);
    for(int i = 0; i < count; i++) {
        char_list = append_to_list(char_list, keys[i]);
    }
    printf("char_list[count - 1] = %s\n", char_list[count - 1]);

    return 0;
}