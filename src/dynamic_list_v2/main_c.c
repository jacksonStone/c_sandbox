#include <stdio.h>
#include "./hash_map.c"

int main() {
    // int * int_list = make_list(int);
    // append_to_list(int_list, 42);
    // printf("int_list[0] = %d\n", int_list[0]);
    // free_list(int_list);
    hash_map map = make_hash_map(char *);
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    for(int i = 0; i < 100; i++) {
        char key[50]; 
        char content[50];
        sprintf(key, "foo%d", i);
        sprintf(content, "Hello%d", i);
        add_hash_entry(map, key, content);
    }
    printf("map.bucket_count = %d\n", map.bucket_count);
    printf("map.entry_count = %d\n", map.entry_count);
    return 0;
}