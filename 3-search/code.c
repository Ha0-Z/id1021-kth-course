#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

long nano_seconds(struct timespec *t_start, struct timespec *t_stop)
{
    return (t_stop->tv_nsec - t_start->tv_nsec) +
           (t_stop->tv_sec - t_start->tv_sec) * 1000000000;
}

bool unsorted_search(int array[], int length, int key) {
    for (int index = 0; index < length ; index++) {
        if (array[index] == key) {
            return true;
        }
    }
    return false;
}

int *sorted(int n) {
    int *array = (int*)malloc(n*sizeof(int));
    int nxt = 0;
    for (int i = 0; i < n ; i++) {
        nxt += rand()%10 + 1;
        array[i] = nxt;
    }
    return array;
}

// int *unsorted_array(int n) {
//     int *array = (int*)malloc(n*sizeof(int));
//     for (int i = 0; i < n ; i++) {
//         array[i] = rand()%10;
//     }
//     return array;
// }
int *unsorted_array(int n) {
    int *array = sorted(n);
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
    return array;
}
bool binary_search(int array[], int length, int key) {
    int first = 0;
    int last = length-1;
    while (true) {
        // jump to the middle
        int index = length/2 ;
        if (array[index] == key) {
            // hmm what now?
            return true;
        }
        if (array[index] < key && index < last) {
            // what is the first possible page?
            first = index;
            continue;
        }
        if (array[index] > key && index > first) {
            // what is the last possible page?
            last = index;
            continue;
        }
        // Why do we land here? What should we do?
        return false;
    }
}
bool recursive(int[] array, int length, int key, int first, int last) {
    // jump to the middle
    int index = first + ((last - first) / 2);
    if (array[index] == key) {
        return true;
    }
    if (array[index] < key && index < last) {
        recursive(array, length, key, index, last);
    }
    if (array[index] > key && index > first) {
        recursive(array, length, key, first, index);
    }
    return false;
}

void bench (int loops, int minItems, int maxItems,int loopGetMinTime) {
    printf("Run of %d loops each try and %d and %d items\n", loops, minItems, maxItems);
    printf("Number of loops, Size of array, Time, Time per loop, time per element\n");
    // Create a array with malloc with i elements.
    for(int arraySize = minItems; arraySize < maxItems; arraySize= arraySize * 2) {
        long timeMin = LONG_MAX;

        // Loop loopGetMinTime times to get the minumum run time.
        for (int i = 0; i < loopGetMinTime; i++) {
            // Fill random items in the dataset
            //
            int *list = unsorted_array(arraySize);

            // Create a for loop which have index of n add doubles after each iteration.
            int *keys = (int*)malloc(loops*sizeof(int));
            for(int n = 1; n <= loops; n++) {
                keys[n] = list[rand()%arraySize];
            }


            struct timespec t_start, t_stop;
            clock_gettime(CLOCK_MONOTONIC, &t_start);
            // 
            // Run the actual code here:
            //

            // Middle element
            for (int x = 0; x < loops; x++) {
                int key = keys[x];
                unsorted_search(list, arraySize, key);
            }

            clock_gettime(CLOCK_MONOTONIC, &t_stop);
            long wall = nano_seconds(&t_start, &t_stop);
            if (wall < timeMin) {
                timeMin = wall;
            }
            free(list);
            free(keys);

        }

        printf("%d %d %0.2fns %0.2fns %0.2fns\n", loops, arraySize, (double)timeMin, (double)timeMin / loops, (double)timeMin / arraySize);
    }
} 

int main () {
    bench(10, 100, 100000, 50);
}