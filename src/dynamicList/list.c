#include <stdlib.h>
#include <stdio.h>
//Add free list
void * _make_list(size_t entry_size, int starting_entries) {
    int malloced = starting_entries * entry_size + (sizeof(int) * 3);
    void * ptr = malloc(malloced);
    char * as_bytes = ptr;
    // Zero it out
    for(int i = 0; i < malloced; i++) {
        as_bytes[i] = 0;
    }
    int * length_ptr = ptr;
    //Space
    *length_ptr = starting_entries;
    //Used
    *(length_ptr + 1) = 0;
    //Size
    *(length_ptr + 2) = entry_size;

    void * returned_pointer = length_ptr + 3;
    return returned_pointer;
}

int get_list_entry_size(void * list_ptr) {
     int* start = list_ptr;
     return *(start - 1);
}
int get_list_length(void * list_ptr) {
     int* start = list_ptr;
     return *(start - 2);
}
int get_max_list_length(void * list_ptr) {
     int* start = list_ptr;
     return *(start - 3);
}
void set_list_length(void * list_ptr, int watermark) {
     int* start = list_ptr;
     *(start - 2) = watermark;
}
void set_max_list_length(void * list_ptr, int length) {
    int* start = list_ptr;
    *(start - 3) = length;
}
void print_list(void * list_ptr) {
    printf("Max capacity: %d\n", get_max_list_length(list_ptr));
    printf("Current Length: %d\n", get_list_length(list_ptr));
    printf("Entry Size: %d\n", get_list_entry_size(list_ptr));
}
void _append(void ** list_ptr_ptr, void * entry) {
    void * list_ptr = * list_ptr_ptr;
    int max = get_max_list_length(list_ptr);
    int cur = get_list_length(list_ptr);
    int size = get_list_entry_size(list_ptr);

    if(cur >= max) {
        void * new_list = realloc((void *) (((int *)list_ptr) - 3), max * 2);
        list_ptr = ((int*) new_list) + 3;
        *list_ptr_ptr = list_ptr;
        set_max_list_length(list_ptr, max * 2);
    }
    char * as_bytes = list_ptr;
    char * new_location = as_bytes + (cur * size);
    char * entry_as_chars = entry;
    for(int i = 0; i < size; i++) {
        new_location[i] = entry_as_chars[i];
    }
    set_list_length(list_ptr, cur + 1);  
}