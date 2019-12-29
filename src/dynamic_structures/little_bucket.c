#include <stdlib.h>
#include "./little_bucket.h"
#include "../dynamic_structures/list.h"
typedef struct little_bucket bucket;
typedef struct bucket_frame frame;
bucket create_bucket() {
    bucket bkt = {};
    bkt.frames = make_list(frame);
    return bkt;
}
bucket create_bucket_with_size(size_t size) {
    bucket bkt = {};
    bkt.total_max = size;
    bkt.frames = make_list_with_starting_length(frame, 1);
    frame first_frame = {};
    first_frame.malloced_ptr = malloc(size);
    first_frame.max = size;
    append_to_list(bkt.frames, first_frame);
    return bkt;
}
void* alloc_bucket(bucket* bkt, size_t size) {
    int needs_new_frame = 0;
    int frame_count = list_entry_count(bkt->frames);
    frame last_frame;
    if (frame_count) {
        last_frame = (bkt->frames)[frame_count - 1];
    }
    if (frame_count && (last_frame.current_byte + size) <= last_frame.max) {
        void * to_return = ((char*)last_frame.malloced_ptr) + last_frame.current_byte;
        (&((bkt->frames)[frame_count - 1]))->current_byte = last_frame.current_byte + size;
        return to_return;
    }
    else {
        //Add frame
        unsigned int new_frame_size;
        if(bkt->total_max > (size*2)) {
            new_frame_size = bkt->total_max;
        } else {
            new_frame_size = size * 2;
        }

        bkt->total_max += new_frame_size;
        frame new_frame = {};
        new_frame.malloced_ptr = malloc(new_frame_size);
        new_frame.max = new_frame_size;
        new_frame.current_byte = size;
        append_to_list(bkt->frames, new_frame);
        return new_frame.malloced_ptr;
    }
}
void free_bucket(bucket* bkt) {
    //Free frames
    int frame_count = list_entry_count(bkt->frames);
    for(int i = 0; i < frame_count; i++) {
        free((bkt->frames)[i].malloced_ptr);
    }
    free_list(bkt->frames);
}