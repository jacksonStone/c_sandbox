#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

typedef struct
{
   unsigned int current_len, limit, signature;
} list_header;
#define list_signature      0x75BCD15

#define make_list(type) _make_list(sizeof(type) * 4)

#define append_to_list(l, entry) \
    _add_to_list((void**) &l, sizeof(entry)),\
    l[(_get_list_header(l)->current_len - sizeof(entry))/sizeof(entry)] = entry

#define _get_list_header(l) (((list_header *) (l)) - 1)

// Returns a pointer to the list
// About 100 nanoseconds to make a list
void* _make_list(int starting_size_in_bytes) {
    list_header* list_head = malloc(starting_size_in_bytes + sizeof(list_header));
    list_header head;
    head.current_len = 0;
    head.limit = starting_size_in_bytes;
    head.signature = list_signature;
    list_head[0] = head;
    return &list_head[1];
}
// Anywhere from 100 nanoseconds per insert (for small) to 6 nanoseconds (for large)
void _add_to_list(void** list_pp, size_t size) {
    list_header* header = _get_list_header(*list_pp);
    //Let's be safe now
    assert(header->signature == list_signature);
    int len = header->current_len;
    if((len + size) > header->limit) {
        header->limit *= 2;
        header = realloc(header, header->limit + sizeof(list_header));
        *list_pp = &header[1];
    }
    header->current_len += size;
    
}
#define free_list(l) _free_list((void *) l)
void _free_list(void* l) {
    free(_get_list_header(l));
}

int main() {
    return 0;
}