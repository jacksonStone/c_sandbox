#include <stdlib.h>
#include <assert.h>

typedef struct
{
   unsigned int current_len, limit, entry_size, signature, freed;
} list_header;
#define list_signature      0x75BCD15
#define DEFAULT_LIST_SPACE  4
#define make_list(type) _make_list(sizeof(type), DEFAULT_LIST_SPACE)
#define make_list_with_starting_length(type, length) _make_list(sizeof(type), length)

#define append_to_list(l, entry) \
    _add_to_list((void**) &l, sizeof(entry)),\
    l[(_get_list_header(l)->current_len - sizeof(entry))/sizeof(entry)] = entry

#define list_entry_count(l) \
 ((_get_list_header(l)->current_len)/(_get_list_header(l)->entry_size))

#define list_already_freed(l) \
 ((_get_list_header(l)->freed))


#define _get_list_header(l) (((list_header *) (l)) - 1)

// Returns a pointer to the list
// About 100 nanoseconds to make a list
void* _make_list(size_t type_size, int entry_count) {
    int starting_size_in_bytes = type_size * entry_count;
    list_header* list_head = malloc(starting_size_in_bytes + sizeof(list_header));
    list_header head;
    head.current_len = 0;
    head.limit = starting_size_in_bytes;
    head.signature = list_signature;
    head.entry_size = type_size;
    head.freed = 0;
    list_head[0] = head;
    return list_head + 1;
}
// Anywhere from 100 nanoseconds per insert (for small) to 6 nanoseconds (for large)
void * _add_to_list(void** list_pp, size_t size) {
    list_header* header = _get_list_header(*list_pp);
    //Let's be safe now
    assert(header->signature == list_signature);
    int len = header->current_len;
    if((len + size) > header->limit) {
        header->limit *= 2;
        header = realloc(header, header->limit + sizeof(list_header));
        //TODO:: Figure out what still needs this!
        *list_pp = header + 1; 
    }
    header->current_len += size;
    return header + 1;
    
}
#define free_list(l) _free_list((void *) l)
void _free_list(void* l) {
    assert(_get_list_header(l)->signature == list_signature);
    assert(!(_get_list_header(l)->freed));
    _get_list_header(l)->freed = 1;
    _get_list_header(l)->signature = 42;
    free(_get_list_header(l));
}