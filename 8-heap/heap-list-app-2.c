#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

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



typedef struct {
    int *array;
    int size;
    int capacity;
} PriorityQueueSortedList;

// Initialize the priority queue
void init(PriorityQueueSortedList *pq) {
    pq->size = 0;
    pq->capacity = 10; // Initial capacity
    pq->array = (int *)malloc(pq->capacity * sizeof(int));
}

// Add an element with O(n) time complexity (maintain sorted order)
void add(PriorityQueueSortedList *pq, int value) {
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->array = (int *)realloc(pq->array, pq->capacity * sizeof(int));
    }

    int i = pq->size - 1;
    while (i >= 0 && pq->array[i] > value) {
        pq->array[i + 1] = pq->array[i];
        i--;
    }
    pq->array[i + 1] = value;
    pq->size++;
}

// Remove the element with the highest priority (smallest number) in O(1) time
int remove_min(PriorityQueueSortedList *pq) {
    if (pq->size == 0) {
        printf("Queue is empty!\n");
        return -1;
    }
    
    int min_value = pq->array[0];
    // Shift all elements to the left
    for (int i = 0; i < pq->size - 1; i++) {
        pq->array[i] = pq->array[i + 1];
    }
    pq->size--;
    return min_value;
}

// Cleanup the priority queue
void free_pq(PriorityQueueSortedList *pq) {
    free(pq->array);
}



// Benchmark adding elements to the heap
void benchmark(int min_size, int max_size, int num_iterations) {
    printf("Benchmarking Add and remove Operations\n");
    printf("Array size      Add(min,µs)     Add(avg/µs)      Rmv(min,µs)     Rmv(avg/µs)     \n");

    for (int size = min_size; size <= max_size; size *= 2) {
        long add_min_time = LONG_MAX;
        long add_total_time = 0;
        long rmv_min_time = LONG_MAX;
        long rmv_total_time = 0;

        PriorityQueueSortedList pq;
        init(&pq);

        for (int iter = 0; iter < num_iterations; iter++) {
            int *array = unsorted_array(size);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                add(&pq, array[i]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long add_time = micro_seconds(&start, &end);
            add_min_time = (add_time < add_min_time) ? add_time : add_min_time;
            add_total_time += add_time;

            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                remove_min(&pq);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long rmv_time = micro_seconds(&start, &end);
            rmv_min_time = (rmv_time < rmv_min_time) ? rmv_time : rmv_min_time;
            rmv_total_time += rmv_time;

            free(array);
        }

        free_pq(&pq);

        printf("%-11d       %-8.0f      %-8.0f     %-8.0f      %-8.0f\n", 
            size, 
            (double)add_min_time,
            (double)add_total_time / num_iterations,
            (double)add_min_time,
            (double)add_total_time / num_iterations
        );
    }
}

int main() {
    benchmark(1000, 128000, 50);
    return 0;
}


// int main() {
//     PriorityQueueSortedList pq;
//     init(&pq);

//     add(&pq, 5);
//     add(&pq, 3);
//     add(&pq, 8);
//     add(&pq, 1);

//     printf("Removed: %d\n", remove_min(&pq)); // Expected: 1
//     printf("Removed: %d\n", remove_min(&pq)); // Expected: 3
//     printf("Removed: %d\n", remove_min(&pq)); // Expected: 5
//     printf("Removed: %d\n", remove_min(&pq)); // Expected: 8

//     free_pq(&pq);
//     return 0;
// }
