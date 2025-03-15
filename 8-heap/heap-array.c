#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Timing functions
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

// Array helper functions
int *sorted_array(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
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

// Array-based Heap Implementation
typedef struct {
    int* array;     // Array to store the heap elements
    int size;       // Current number of elements in the heap
    int capacity;   // Maximum capacity of the heap
} Heap;

// Create a new heap with initial capacity
Heap* create_heap(int capacity) {
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    if (heap == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    heap->array = (int*)malloc(capacity * sizeof(int));
    if (heap->array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(heap);
        exit(1);
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Swap two elements in the array
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Get parent index for node at index i
int parent(int i) {
    if (i % 2 == 0) {        // even number
        return (i - 2) / 2; 
    } else {                  // odd number
        return (i - 1) / 2;
    }
    // Simplified: return (i - 1) / 2;
}

// Get left child index for node at index i
int left_child(int i) {
    return 2 * i + 1;
}

// Get right child index for node at index i
int right_child(int i) {
    return 2 * i + 2;
}

// "Bubble up" (sift up) process - move a value up to maintain heap property
void bubble_up(Heap* heap, int index) {
    // If we're at the root or parent is smaller, we're done
    if (index == 0 || heap->array[parent(index)] <= heap->array[index]) {
        return;
    }
    
    // Swap with parent and continue bubbling up
    swap(&heap->array[index], &heap->array[parent(index)]);
    bubble_up(heap, parent(index));
}

// "Sink down" (sift down) process - move a value down to maintain heap property
void sink_down(Heap* heap, int index) {
    int smallest = index;
    int left = left_child(index);
    int right = right_child(index);
    
    // Check if left child exists and is smaller than current smallest
    if (left < heap->size && heap->array[left] < heap->array[smallest]) {
        smallest = left;
    }
    
    // Check if right child exists and is smaller than current smallest
    if (right < heap->size && heap->array[right] < heap->array[smallest]) {
        smallest = right;
    }
    
    // If smallest isn't the current index, swap and continue sinking
    if (smallest != index) {
        swap(&heap->array[index], &heap->array[smallest]);
        sink_down(heap, smallest);
    }
}

// Add a value to the heap
void add(Heap* heap, int value) {
    // Check if heap is full and resize if needed
    if (heap->size >= heap->capacity) {
        heap->capacity *= 2;
        heap->array = (int*)realloc(heap->array, heap->capacity * sizeof(int));
        if (heap->array == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }
    
    // Place the new element at the end of the array
    heap->array[heap->size] = value;
    
    // Bubble up the new element to maintain heap property
    bubble_up(heap, heap->size);
    
    // Increment size
    heap->size++;
}

// Remove and return the minimum element (root)
int remove_min(Heap* heap) {
    if (heap->size <= 0) {
        fprintf(stderr, "Heap is empty\n");
        return -1; // Error value
    }
    
    // Get the root value (minimum)
    int min_val = heap->array[0];
    
    // Replace root with the last element
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    
    // Sink down the new root to maintain heap property
    if (heap->size > 0) {
        sink_down(heap, 0);
    }
    
    return min_val;
}

// Push operation: increment the root element and let it sink to its right position
int push(Heap* heap, int incr, int depth) {
    if (heap->size <= 0) {
        return depth;
    }
    
    // Increment the root value
    heap->array[0] += incr;
    
    // Sink the root to the appropriate position
    int current = 0;
    while (1) {
        int smallest = current;
        int left = left_child(current);
        int right = right_child(current);
        
        if (left < heap->size && heap->array[left] < heap->array[smallest]) {
            smallest = left;
        }
        
        if (right < heap->size && heap->array[right] < heap->array[smallest]) {
            smallest = right;
        }
        
        if (smallest == current) {
            break; // No more sinking needed
        }
        
        // Swap and update current index
        swap(&heap->array[current], &heap->array[smallest]);
        current = smallest;
        depth++;
    }
    
    return depth;
}

// Print the heap (for debugging)
void print_heap(Heap* heap) {
    printf("Heap contents: ");
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->array[i]);
    }
    printf("\n");
}

// Free the heap
void free_heap(Heap* heap) {
    if (heap) {
        free(heap->array);
        free(heap);
    }
}

// Benchmark - comparing push vs remove-add operations
void benchmark_push_vs_remove_add(int size, int num_operations, int incr_min, int incr_max) {
    printf("Benchmarking Push vs Remove-Add with %d elements\n", size);
    printf("Performing %d operations\n", num_operations);
    
    // Initialize random numbers
    int* random_values = unsorted_array(size);
    
    // Create two identical heaps
    Heap* heap_push = create_heap(size);
    Heap* heap_remove_add = create_heap(size);
    
    // Fill both heaps with the same values
    for (int i = 0; i < size; i++) {
        add(heap_push, random_values[i]);
        add(heap_remove_add, random_values[i]);
    }
    
    // Generate random increments
    int* increments = (int*)malloc(num_operations * sizeof(int));
    for (int i = 0; i < num_operations; i++) {
        increments[i] = rand() % (incr_max - incr_min + 1) + incr_min;
    }
    
    // Benchmark push operation
    int total_push_depth = 0;
    struct timespec push_start, push_end;
    clock_gettime(CLOCK_MONOTONIC, &push_start);
    
    for (int i = 0; i < num_operations; i++) {
        total_push_depth += push(heap_push, increments[i], 0);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &push_end);
    long push_time = micro_seconds(&push_start, &push_end);
    double avg_push_depth = (double)total_push_depth / num_operations;
    
    // Benchmark remove-add operation
    struct timespec remove_add_start, remove_add_end;
    clock_gettime(CLOCK_MONOTONIC, &remove_add_start);
    
    for (int i = 0; i < num_operations; i++) {
        int value = remove_min(heap_remove_add);
        add(heap_remove_add, value + increments[i]);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &remove_add_end);
    long remove_add_time = micro_seconds(&remove_add_start, &remove_add_end);
    
    // Print results
    printf("Push operation:\n");
    printf("  - Total time: %ld µs\n", push_time);
    printf("  - Average depth: %.2f levels\n", avg_push_depth);
    
    printf("Remove-Add operation:\n");
    printf("  - Total time: %ld µs\n", remove_add_time);
    
    printf("Performance ratio (Push:Remove-Add): 1:%.2f\n", (double)remove_add_time / push_time);
    
    // Clean up
    free(random_values);
    free(increments);
    free_heap(heap_push);
    free_heap(heap_remove_add);
}

// Benchmark adding and removing elements
void benchmark_add_remove(int min_size, int max_size, int num_iterations) {
    printf("Benchmarking Add and Remove Operations\n");
    printf("Array size      Add(min,µs)     Add(avg/µs)      Rmv(min,µs)     Rmv(avg/µs)     \n");

    for (int size = min_size; size <= max_size; size *= 2) {
        long add_min_time = LONG_MAX;
        long add_total_time = 0;
        long rmv_min_time = LONG_MAX;
        long rmv_total_time = 0;

        for (int iter = 0; iter < num_iterations; iter++) {
            int *array = unsorted_array(size);
            Heap* heap = create_heap(size);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                add(heap, array[i]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long add_time = micro_seconds(&start, &end);
            add_min_time = (add_time < add_min_time) ? add_time : add_min_time;
            add_total_time += add_time;

            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                remove_min(heap);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long rmv_time = micro_seconds(&start, &end);
            rmv_min_time = (rmv_time < rmv_min_time) ? rmv_time : rmv_min_time;
            rmv_total_time += rmv_time;

            free_heap(heap);
            free(array);
        }

        printf("%-11d       %-8ld      %-8.0f     %-8ld      %-8.0f\n", 
            size, 
            add_min_time,
            (double)add_total_time / num_iterations,
            rmv_min_time,
            (double)rmv_total_time / num_iterations
        );
    }
}

int main() {
    srand(time(NULL));
    
    // Basic benchmark of add/remove operations
    benchmark_add_remove(1000, 128000, 10);
    
    printf("\n");
    
    // Benchmark comparing push vs remove-add operations
    benchmark_push_vs_remove_add(1023, 1000, 10, 100);
    
    return 0;
}