#include "../timer/timer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    start_timer();
    int size = 500000;
    char* foo = malloc(size);
    char* boo = malloc(size);

    for (int i = 0; i < size; i++) {
        foo[i] = 'a';
    }

    for (int i = 0; i < size; i++) {
        boo[i] = foo[i];
    }
    
    double duration = stop_timer();
    printf("\nC duration in-order access is: %f milliseconds\n", duration/1000);
    
    int* shuffled_list = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        shuffled_list[i] = rand() % size;
    }

    start_timer();
    char* foo2 = malloc(size);
    char* boo2 = malloc(size);

    for (int i = 0; i < size; i++) {
        foo2[shuffled_list[i]] = 'a';
    }

    for (int i = 0; i < size; i++) {
        boo2[shuffled_list[i]] = foo2[shuffled_list[i]];
    }
    
    double duration2 = stop_timer();
    printf("C duration out-of-order access is: %f milliseconds\n", duration2/1000);
    return 0;
}