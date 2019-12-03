#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>

struct {
    unsigned int max;
    unsigned int current_byte;
    void* initial_malloc_ptr;
} typedef little_stack;

little_stack create_stack() {
    little_stack stk = {};
    return stk;
}
static int called_realloc = 0;
void* alloc_stack(little_stack* stk, size_t size) {
    int should_realloc = 0;
    while((size + stk->current_byte) > stk->max) {
        // Realloc        
        if(stk->max == 0) {
            stk->max = 2;
        } else {
            stk->max = stk->max * 2;
        }
        should_realloc = 1;
    }
    if (should_realloc) {
        void* old_ptr = stk->initial_malloc_ptr;
        if (old_ptr) {
            stk->initial_malloc_ptr = realloc(old_ptr, stk->max);
        } else {
            stk->initial_malloc_ptr = malloc(stk->max);
        }
        called_realloc++;
    }
    void* next_ptr = ((char *) (stk->initial_malloc_ptr)) + stk->current_byte;
    stk->current_byte = stk->current_byte + size;
    return next_ptr;
}

char * stuff_with_alloc(little_stack* stk, char c) {
    char * foo = alloc_stack(stk, 20);
    // for(int i = 0; i < 19; i++) {
    //     foo[i] = c;
    // }
    foo[19] = '\0';
    return foo;
}

char * stuff_with_alloc_no_stack(char c) {
    char * foo = malloc(20);
    // for(int i = 0; i < 19; i++) {
    //     foo[i] = c;
    // }
    foo[19] = '\0';
    return foo;
}

static struct timeval t0;
static struct timeval t1;
void start_timer(void) {
    gettimeofday(&t0, 0);
}
//Time elapsed in microseconds
double stop_timer(void) {
    gettimeofday(&t1, 0);
    return (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;
}

int main() {
    char * str = "abcdefghij";
    int interations = 10000;

    start_timer();
    for(int i = 0; i < interations; i++) {
        char * str_cpy = str;
        while(*str_cpy) {
            stuff_with_alloc_no_stack(*str_cpy);
            str_cpy++;
        }
    }
    printf("MALLOC:\t\t Time to do: %f\n", stop_timer());


    start_timer();
    little_stack stk = create_stack(); 
    for(int i = 0; i < interations; i++) {
        char * str_cpy = str;
        while(*str_cpy) {
            stuff_with_alloc(&stk, *str_cpy);
            str_cpy++;
        }
    }
    printf("STACK:\t\t Time to do: %f, reallocs: %d\n", stop_timer(), called_realloc);
    
    start_timer();
    char real_stack[interations][10][20];
    for(int i = 0; i < interations; i++) {
        char * str_cpy = str;
        int j = 0;
        while(*str_cpy) {
            char * foo = real_stack[i][j];
            // j++;
            // for(int k = 0; k < 19; k++) {
            //     foo[k] = *str_cpy;
            // }
            foo[19] = '\0';
            str_cpy++;
        }
    }
    printf("REAL STACK:\t\t Time to do: %f\n", stop_timer());
    
    return 0;
}