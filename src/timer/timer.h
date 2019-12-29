#ifndef TIMER_ADDED
#define TIMER_ADDED
#include <sys/time.h>
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
#endif /* TIMER_ADDED */