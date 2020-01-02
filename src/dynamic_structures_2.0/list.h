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
#define make_list_with_allocator(type, gh) _make_list(sizeof(type), 8, 0, gh)
#define make_list_with_allocator_with_size(type, size, gh) _make_list(sizeof(type), size, 0, gh)
#define make_list(type) _make_allocator_then_list(sizeof(type), 8)
#define make_list_with_size(type, count) _make_allocator_then_list(sizeof(type), sizeof(type) * (count))
#define free_list(l) _free_list(_get_list_header((l)));
#define get_list_len(l) _get_list_header((l))->len
#define _get_list_header(l) (((js_list_header *) (l)) - 1)
#define append_to_list(l, entry) \
    _add_to_list((void**) &(l), sizeof(entry)),\
    (l)[_get_list_header((l))->len - 1] = entry


void _free_list(js_list_header* header) {
    if(header->can_be_freed) {
        gh_free(header->gh);
        free(header->gh);
    }
}

void* _add_to_list(void ** list_ptr, size_t size_to_add) {
    js_list_header* header_ptr = _get_list_header(*list_ptr);
    js_list_header header = *header_ptr;
    //Let's be safe now
    assert(header_ptr->signature == signature_for_js_list);
    if(header_ptr->memory_usage + size_to_add > header_ptr->memory_limit) {
        header_ptr->memory_limit *= 2;
        size_t new_size = header_ptr->memory_limit + sizeof(js_list_header);
        growing_heap* gh;
        if(header_ptr->can_be_freed) {
            gh = malloc(sizeof(growing_heap));
            *gh = make_growing_heap();
        } else {
            gh = header_ptr->gh;
        }
        js_list_header* new_header = gh_malloc(gh, new_size);
        memcpy(new_header, header_ptr, new_size);
        _free_list(header_ptr);
        if(new_header->can_be_freed) {
            new_header-> gh = gh;
        }
        header_ptr = new_header;
        *list_ptr = new_header + 1;
    }
    header_ptr->memory_usage += size_to_add;
    header_ptr->len += 1;
    return header_ptr + 1;
}

void* _make_list(size_t entry_size, size_t entry_count, int can_be_freed, growing_heap* gh) {
    size_t size = entry_size * entry_count;
    js_list_header* entry = gh_malloc(gh, size + sizeof(js_list_header));
    js_list_header header = {
        0,
        size,
        entry_size,
        0,
        can_be_freed,
        signature_for_js_list,
        gh
    };
    entry[0] = header;
    return (void*) &(entry[1]);
}
void* _make_allocator_then_list(size_t entry_size, size_t entry_count) {
    growing_heap* gh_ptr = malloc(sizeof(growing_heap));
    *gh_ptr = make_growing_heap();
    return _make_list(entry_size, entry_count, 1, gh_ptr);
}


#endif // JS_LIST
