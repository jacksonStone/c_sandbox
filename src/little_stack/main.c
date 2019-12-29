#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "../timer/timer.h"
#include "../dynamic_structures/little_stack.h"

char * stuff_with_alloc(struct little_stack* stk, char c) {
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
    struct little_stack stk = create_stack(); 
    int inter = interations*10;
    char* res[inter];
    int j = 0;
    for(int i = 0; i < interations; i++) {
        char * str_cpy = str;
        while (*str_cpy) {
            res[j] = stuff_with_alloc(&stk, *str_cpy);
            j++;
            str_cpy++;
        }
    }
    printf("STACK:\t\t Time to do: %f\n", stop_timer());
    printf("result:\t\t %s\n", res[0]);
    printf("result:\t\t %s\n", res[interations+1]);
    printf("result:\t\t %s\n", res[interations*10 - 1]);
    
    start_timer();
    char real_stack[interations][10][20];
    for(int i = 0; i < interations; i++) {
        char * str_cpy = str;
        int j = 0;
        while(*str_cpy) {
            char * foo = real_stack[i][j];
            j++;
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