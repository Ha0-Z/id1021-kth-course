#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}

int *sorted(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
    int current_value = 0;
    for (int i = 0; i < size; i++) {
        current_value += rand() % 10 + 1;
        numbers[i] = current_value;
    }
    return numbers;
}

int *unsorted_array(int size) {
    int *numbers = sorted(size);
    for (int i = 0; i < size; i++) {
        int random_index = rand() % size;
        int temp = numbers[i];
        numbers[i] = numbers[random_index];
        numbers[random_index] = temp;
    }
    return numbers;
}


void bench(int num_searches, int min_array_size, int max_array_size, int num_iterations) {
    printf("Running %d searches on arrays from size %d to %d\n", num_searches, min_array_size, max_array_size);
    printf("Loops   Array size   TimeMin   TimeMax   Average\n");

    for(int current_size = min_array_size; current_size < max_array_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {

            // DATASET
            int *test_array = unsorted_array(current_size);

            int *search_targets = (int*)malloc(num_searches * sizeof(int));
            for(int i = 1; i < num_searches; i++) {
                search_targets[i] = test_array[rand() % current_size];
            }


            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // CODE TO BENCHMARK
            for (int search_count = 0; search_count < num_searches; search_count++) {


            }

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = nano_seconds(&start_time, &end_time);
            
            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;
            
            free(test_array);
            free(search_targets);
        }

        printf("%d   %d   %0.2fns   %0.2fns   %0.2fns \n", 
            num_searches, current_size, 
            (double)min_time / num_searches, 
            (double)max_time / num_searches, 
            (double)total_time / num_iterations / num_searches);
    }
} 

int main() {
    bench(1000, 10000, 2000000, 50);
    return 0;
}