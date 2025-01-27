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

void bench (int minLoops, int maxLoops, int minItems, int maxItems,int loopGetMinTime) {
    printf("Run time between %d and %d loops and %d and %d items\n", minLoops, maxLoops, minItems, maxItems);
    printf("Number of loops, Size of array, Time, Time per loop\n");
    // Create a for loop which have index of n add doubles after each iteration.
    for(int n = minLoops; n <= maxLoops; n = n * 2) {
        // Create a array with malloc with i elements.
        for(int arraySize = minItems; arraySize < maxItems; arraySize= arraySize * 2) {
            long timeMin = LONG_MAX;

            // Loop loopGetMinTime times to get the minumum run time.
            for (int i = 0; i < loopGetMinTime; i++) {
                // Fill random items in the dataset
                //


                struct timespec t_start, t_stop;
                clock_gettime(CLOCK_MONOTONIC, &t_start);
                for (int x = 0; x < n; x++) {
                    // Run the actual code here:
                    //

                }

                clock_gettime(CLOCK_MONOTONIC, &t_stop);
                long wall = nano_seconds(&t_start, &t_stop);
                if (wall < timeMin) {
                    timeMin = wall;
                }
                free(list);

            }

            printf("%d %d %0.2f %0.2f ns\n", n, arraySize, (double)timeMin, (double)timeMin / n);
        }
    }
} 
