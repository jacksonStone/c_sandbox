#ifndef JS_LIST
#define JS_LIST
#include "./growing_heap.h"
#include <assert.h>
typedef struct
{
   unsigned int memory_usage, 
    memory_limit,
    entry_size,
    len,
    can_be_freed,
    signature;
   growing_heap* gh;
} js_list_header;
#define signature_for_js_list 1234567
#define make_wasteful_list(type, gh) _make_list(sizeof(type), 8, &gh)
#define _get_list_header(l) (((js_list_header *) (l)) - 1)
#define append_to_list(l, entry) \
    _add_to_list((void**) &(l), sizeof(entry)),\
    (l)[_get_list_header((l))->len - 1] = entry
void* _add_to_list(void ** list_ptr, size_t size_to_add) {
    js_list_header* header_ptr = _get_list_header(*list_ptr);
    js_list_header header = *header_ptr;
    //Let's be safe now
    assert(header.signature == signature_for_js_list);
    if(header.memory_usage + size_to_add > header.memory_limit) {
        header_ptr->memory_limit *= 2;
        size_t new_size = header_ptr->memory_limit + sizeof(js_list_header);
        js_list_header* new_header = gh_malloc(header.gh, new_size);
        memcpy(new_header, header_ptr, new_size);
        header_ptr = new_header;
        //TODO:: Potientially deal with mem copy not cleaning up old list
        *list_ptr = new_header + 1;
    }
    header_ptr->memory_usage += size_to_add;
    header_ptr->len += 1;
    return header_ptr + 1;
}
void* _make_list(size_t entry_size, size_t entry_count, growing_heap* gh) {
    size_t size = entry_size * entry_count;
    js_list_header* entry = gh_malloc(gh, size + sizeof(js_list_header));
    js_list_header header = {
        0,
        size,
        entry_size,
        0,
        0,
        signature_for_js_list,
        gh
    };
    entry[0] = header;
    return (void*) &(entry[1]);
}


#endif // JS_LIST
