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



// Define the heap structure
typedef struct {
    int *array;
    int size;
    int capacity;
} MinHeap;

// Initialize the heap
void init(MinHeap *heap, int capacity) {
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (int *)malloc(capacity * sizeof(int));
}

// Swap two elements in the heap
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify the tree (maintain the heap property)
void heapify(MinHeap *heap, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < heap->size && heap->array[left] < heap->array[smallest]) {
        smallest = left;
    }
    if (right < heap->size && heap->array[right] < heap->array[smallest]) {
        smallest = right;
    }

    if (smallest != index) {
        swap(&heap->array[index], &heap->array[smallest]);
        heapify(heap, smallest);
    }
}

// Insert a value into the heap
void insert(MinHeap *heap, int value) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->array = (int *)realloc(heap->array, heap->capacity * sizeof(int));
    }

    heap->array[heap->size] = value;
    int index = heap->size;
    heap->size++;

    // Fix the heap property by "bubbling up"
    while (index > 0 && heap->array[(index - 1) / 2] > heap->array[index]) {
        swap(&heap->array[index], &heap->array[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

// Remove the root (smallest element) from the heap
int remove_min(MinHeap *heap) {
    if (heap->size == 0) {
        printf("Heap is empty!\n");
        return -1;
    }

    // Get the root (smallest element)
    int root = heap->array[0];

    // Replace root with the last element in the heap
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    // Restore the heap property by "bubbling down"
    heapify(heap, 0);

    return root;
}

// Print the heap array (for debugging)
void print_heap(MinHeap *heap) {
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->array[i]);
    }
    printf("\n");
}

// Free the heap memory
void free_heap(MinHeap *heap) {
    free(heap->array);
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

    MinHeap heap;
    init(&heap, size);

        for (int iter = 0; iter < num_iterations; iter++) {
            int *array = unsorted_array(size);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                insert(&heap, array[i]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long add_time = micro_seconds(&start, &end);
            add_min_time = (add_time < add_min_time) ? add_time : add_min_time;
            add_total_time += add_time;

            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                remove_min(&heap);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long rmv_time = micro_seconds(&start, &end);
            rmv_min_time = (rmv_time < rmv_min_time) ? rmv_time : rmv_min_time;
            rmv_total_time += rmv_time;

            free(array);
        }

        free_heap(&heap);

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
    srand(time(NULL));

    benchmark(1000, 128000, 50);
    
    return 0;
}
