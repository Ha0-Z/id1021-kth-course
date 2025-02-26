#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

#include "tree.c"

long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}

long micro_seconds(struct timespec *start, struct timespec *end) {
    long sec_diff = end->tv_sec - start->tv_sec;
    long nsec_diff = end->tv_nsec - start->tv_nsec;
    
    if (nsec_diff < 0) {
        sec_diff--;
        nsec_diff += 1000000000;
    }
    return (sec_diff * 1000000) + (nsec_diff / 1000);
}

int *sorted_array(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
    int current_value = 0;
    for (int i = 0; i < size; i++) {
        current_value += rand() % 10 + 1;
        numbers[i] = current_value;
    }
    return numbers;
}

int *unsorted_array(int size) {
    int *numbers = sorted_array(size);
    for (int i = 0; i < size; i++) {
        int random_index = rand() % size;
        int temp = numbers[i];
        numbers[i] = numbers[random_index];
        numbers[random_index] = temp;
    }
    return numbers;
}

// Benchmark function for adding elements to the tree
void bench_add(int min_tree_size, int max_tree_size, int num_iterations, char* add_function_name, void (*add_func)(tree*, int)) {
    printf("Benchmarking '%s' add operation on trees from size %d to %d\n", add_function_name, min_tree_size, max_tree_size);
    printf("Loops   Tree size   TimeMin (ns)   TimeMax (ns)   Average (ns)\n");

    for (int current_size = min_tree_size; current_size <= max_tree_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // DATASET - Unsorted array to build the tree
            int *test_array = unsorted_array(current_size);
            tree *test_tree = construct_tree();

            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // CODE TO BENCHMARK - Add elements to the tree
            for (int i = 0; i < current_size; i++) {
                add_func(test_tree, test_array[i]);
            }

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = nano_seconds(&start_time, &end_time);

            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;

            free(test_array);
            free_tree(test_tree); // Free the tree after each iteration
        }

        printf("%d   %d   %0.2f   %0.2f   %0.2f \n",
               num_iterations, current_size,
               (double)min_time / current_size, // Time per element added
               (double)max_time / current_size, // Time per element added
               (double)total_time / num_iterations / current_size); // Average time per element added
    }
}

// Benchmark function for lookup operations
void bench_lookup(int num_searches, int min_tree_size, int max_tree_size, int num_iterations, char* lookup_function_name, bool (*lookup_func)(tree*, int)) {
    printf("Benchmarking '%s' lookup operation (%d searches per iteration) on trees from size %d to %d\n",
           lookup_function_name, num_searches, min_tree_size, max_tree_size);
    printf("Loops   Tree size   TimeMin (ns)   TimeMax (ns)   Average (ns)\n");

    for (int current_size = min_tree_size; current_size <= max_tree_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // DATASET - Unsorted array to build the tree and search targets
            int *test_array = unsorted_array(current_size);
            tree *test_tree = construct_tree();
            for (int i = 0; i < current_size; i++) {
                add_while(test_tree, test_array[i]); // Using add_while for tree construction in lookup benchmark for consistency, can be changed.
            }

            int *search_targets = (int*)malloc(num_searches * sizeof(int));
            for (int i = 0; i < num_searches; i++) {
                search_targets[i] = test_array[rand() % current_size]; // Search for values that are likely in the tree
            }

            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // CODE TO BENCHMARK - Lookup operations
            for (int search_count = 0; search_count < num_searches; search_count++) {
                lookup_func(test_tree, search_targets[search_count]);
            }

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = nano_seconds(&start_time, &end_time);

            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;

            free(test_array);
            free_tree(test_tree);
            free(search_targets);
        }

        printf("%d   %d   %0.2f   %0.2f   %0.2f \n",
               num_iterations, current_size,
               (double)min_time / num_searches, // Time per search
               (double)max_time / num_searches, // Time per search
               (double)total_time / num_iterations / num_searches); // Average time per search
    }
}

// Benchmark function for tree printing (in-order traversal)
void bench_print(int min_tree_size, int max_tree_size, int num_iterations, char* print_function_name, void (*print_func)(tree*)) {
    printf("Benchmarking '%s' print operation on trees from size %d to %d\n", print_function_name, min_tree_size, max_tree_size);
    printf("Loops   Tree size   TimeMin (µs)   TimeMax (µs)   Average (µs)\n");

    for (int current_size = min_tree_size; current_size <= max_tree_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // DATASET - Unsorted array to build the tree
            int *test_array = unsorted_array(current_size);
            tree *test_tree = construct_tree();
            for (int i = 0; i < current_size; i++) {
                add_while(test_tree, test_array[i]); // Using add_while for tree construction in print benchmark for consistency, can be changed.
            }

            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // CODE TO BENCHMARK - Print operation
            print_func(test_tree);

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = micro_seconds(&start_time, &end_time); // Microseconds for print as it might be faster

            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;

            free(test_array);
            free_tree(test_tree);
        }

        printf("%d   %d   %0.2f   %0.2f   %0.2f \n",
               num_iterations, current_size,
               (double)min_time,
               (double)max_time,
               (double)total_time / num_iterations);
    }
}


int main() {
    srand(time(NULL)); // Seed random number generator

    printf("--- Benchmarking Add Operations ---\n");
    bench_add(1000, 128000, 50, "add_while", add_while);
    bench_add(1000, 128000, 50, "add_recursive", add); // Assuming 'add' is your recursive add function

    printf("\n--- Benchmarking Lookup Operations ---\n");
    bench_lookup(1000, 1000, 128000, 50, "lookup_while", lookup_while);
    bench_lookup(1000, 1000, 128000, 50, "lookup_while", lookup);

    printf("\n--- Benchmarking Print Operations ---\n");
    bench_print(1000, 128000, 50, "print_tree", print_tree); // Recursive print
    bench_print(1000, 128000, 50, "print_tree_explicit_stack", print_tree_explicit_stack); // Iterative print

    return 0;
}