#include <stdlib.h>
#include "./little_stack.h"
#include "../dynamic_structures/list.h"
typedef struct little_stack stack;
typedef struct stack_frame frame;
stack create_stack() {
    stack stk = {};
    stk.frames = make_list(frame);
    return stk;
}
void* alloc_stack(stack* stk, size_t size) {
    int needs_new_frame = 0;
    int frame_count = list_entry_count(stk->frames);
    frame last_frame; 
    if (frame_count) {
        last_frame = (stk->frames)[frame_count - 1];
    }
    if (frame_count && (last_frame.current_byte + size) <= last_frame.max) {
        void * to_return = ((char*)last_frame.malloced_ptr) + last_frame.current_byte;
        (&((stk->frames)[frame_count - 1]))->current_byte = last_frame.current_byte + size;
        return to_return;
    }
    else {
        //Add frame
        unsigned int new_frame_size;
        if(stk->total_max > (size*2)) {
            new_frame_size = stk->total_max;
        } else {
            new_frame_size = size * 2;
        }

        stk->total_max += new_frame_size;
        frame new_frame = {};
        new_frame.malloced_ptr = malloc(new_frame_size);
        new_frame.max = new_frame_size;
        new_frame.current_byte = size;
        append_to_list(stk->frames, new_frame);
        return new_frame.malloced_ptr;
    }
}
void free_stack(stack* stk) {
    //Free frames
    int frame_count = list_entry_count(stk->frames);
    for(int i = 0; i < frame_count; i++) {
        free((stk->frames)[i].malloced_ptr);
    }
    free_list(stk->frames);
}