#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>

long nano_seconds(struct timespec *t_start, struct timespec *t_stop)
{
    return (t_stop->tv_nsec - t_start->tv_nsec) +
           (t_stop->tv_sec - t_start->tv_sec) * 1000000000;
}

// A generic benchmark function that accepts a function pointer
long benchmark(void (*func), int loop)
{
    struct timespec t_start, t_stop;

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < loop; i++) {
        func();  // Call the function that you want to benchmark
    }
    clock_gettime(CLOCK_MONOTONIC, &t_stop);

    long wall = nano_seconds(&t_start, &t_stop);
    return wall;
}
