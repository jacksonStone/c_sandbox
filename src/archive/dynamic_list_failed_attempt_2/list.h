#include <stdlib.h>
#define INITIAL_DEFAULT_LIST_CAPACITY 8
void * _make_list(size_t entry_size, int starting_entries);
int get_list_length(void * list_ptr);
void set_list_length(void * list_ptr, int len);
void print_list(void * list_ptr);
void _append(void ** list_ptr_ptr, void * entry);
#define append(list_ptr, entry) (_append((void**)&list_ptr, (void*) &(entry)))
#define make_list(type) _make_list(sizeof(type), INITIAL_DEFAULT_LIST_CAPACITY)
#define make_list_with_initial_capacity(type, length) _make_list(sizeof(type), length)