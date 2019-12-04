struct stack_frame {
    unsigned int max;
    unsigned int current_byte;
    void* malloced_ptr;
};
struct little_stack {
    unsigned int total_max;
    struct stack_frame* frames;
};

struct little_stack create_stack(void);
void* alloc_stack(struct little_stack* stk, size_t size);
void free_stack(struct little_stack* stk);