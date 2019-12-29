#ifndef little_bucket_header
#define little_bucket_header

struct bucket_frame {
    unsigned int max;
    unsigned int current_byte;
    void* malloced_ptr;
};
struct little_bucket {
    unsigned int total_max;
    struct bucket_frame* frames;
};

struct little_bucket create_bucket(void);
struct little_bucket create_bucket_with_size(size_t);
void* alloc_bucket(struct little_bucket* bkt, size_t size);
void free_bucket(struct little_bucket* bkt);
#endif //little_bucket_header