#include <stdlib.h>
typedef struct
{
   unsigned int current_len, limit, entry_size, signature;
} list_header;
#define DEFAULT_LIST_SPACE  4
#define make_list(type) _make_list(sizeof(type), DEFAULT_LIST_SPACE)
#define make_list_with_starting_length(type, length) _make_list(sizeof(type), length)
#define append_to_list(l, entry) \
    _add_to_list((void**) &(l), sizeof(entry)),\
    (l)[(_get_list_header((l))->current_len - sizeof(entry))/sizeof(entry)] = entry
#define list_entry_count(l) \
 ((_get_list_header(l)->current_len)/(_get_list_header(l)->entry_size))
#define _get_list_header(l) (((list_header *) (l)) - 1)

// Returns a pointer to the list
// About 100 nanoseconds to make a list
void* _make_list(size_t type_size, int entry_count);
// Anywhere from 100 nanoseconds per insert (for small) to 6 nanoseconds (for large)
void * _add_to_list(void** list_pp, size_t size);
#define free_list(l) _free_list((void *) l)
void _free_list(void* l);