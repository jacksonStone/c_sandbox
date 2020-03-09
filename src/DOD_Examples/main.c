#include "../timer/timer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    start_timer();
    int size = 5000000;
    char* foo = malloc(size*2);
    char* boo = &foo[size];

    for (int i = 0; i < size; i++) {
        foo[i] = 'a';
    }

    for (int i = 0; i < size; i++) {
        boo[i] = foo[i];
    }
    
    double duration = stop_timer();
    printf("\nC duration in-order access is: %f milliseconds\n", duration/1000);

    int * randNums = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        randNums[i] = (rand() % size);
    }

    start_timer();
    char* foo3 = malloc(size*2);
    char* boo3 = &foo3[size];

    for (int i = 0; i < size; i++) {
        foo3[randNums[i]] = 'a';
    }

    for (int i = 0; i < size; i++) {
        boo3[randNums[i]] = foo3[randNums[i]];
    }
    
    double duration3 = stop_timer();
    printf("C duration out-of-order access with random array is: %f milliseconds\n", (duration3)/1000);
    printf("Runtime ratio: %f\n", (duration3/duration));

    return 0;
}