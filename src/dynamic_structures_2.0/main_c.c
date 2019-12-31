#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../timer/timer.h"
#include "./types.h"
#include "./static_heap.h"
#include "./growing_heap.h"
#include "./list.h"
#include "./hashmap.h"

int main() {
    int allot_size = 500;
    start_timer();
    growing_heap gh = make_growing_heap_with_size(allot_size*32);
    char* my_str = gh_malloc(&gh, allot_size);
    int* my_ints = make_list_with_allocator(int, &gh);
    my_ints = append_to_list(my_ints, 4);
    hashmap hm = make_hashmap(&gh);
    int k = 4;
    add_to_hashmap_c_string(&hm, "zz1", &k);
    int * res = get_hashmap_entry_c_string(&hm, "zz1");
    printf("Just retrieved: %d\n", *res);

    //Is a no-op if you pass an allocator during list creation
    free_list(my_ints);
    gh_free(&gh);
    printf("time: %f microsec\n", stop_timer());
    return 0;
}

