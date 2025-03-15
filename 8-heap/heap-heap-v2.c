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

// ===== Heap Implementation: Array-Based =====

// More efficient array-based min-heap implementation
typedef struct {
    int* data;     // Array to store the heap
    int capacity;  // Maximum capacity of the heap
    int size;      // Current number of elements in the heap
} MinHeap;

// Create a new min heap
MinHeap* create_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (heap == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    heap->data = (int*)malloc(capacity * sizeof(int));
    if (heap->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(heap);
        exit(1);
    }
    
    heap->capacity = capacity;
    heap->size = 0;
    return heap;
}

// Get parent index
int parent(int i) {
    return (i - 1) / 2;
}

// Get left child index
int left_child(int i) {
    return 2 * i + 1;
}

// Get right child index
int right_child(int i) {
    return 2 * i + 2;
}

// Swap two elements in the array
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify (sift-down): maintain heap property from top to bottom
void heapify(MinHeap* heap, int i) {
    int smallest = i;
    int left = left_child(i);
    int right = right_child(i);
    
    // Find the smallest among node i, its left child, and right child
    if (left < heap->size && heap->data[left] < heap->data[smallest])
        smallest = left;
    
    if (right < heap->size && heap->data[right] < heap->data[smallest])
        smallest = right;
    
    // If the smallest isn't i, swap and continue heapifying
    if (smallest != i) {
        swap(&heap->data[i], &heap->data[smallest]);
        heapify(heap, smallest);
    }
}

// Insert a new element into the heap
void insert(MinHeap* heap, int value) {
    if (heap->size >= heap->capacity) {
        // Increase capacity if needed
        heap->capacity *= 2;
        heap->data = (int*)realloc(heap->data, heap->capacity * sizeof(int));
        if (heap->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }
    
    // Insert at the end
    int i = heap->size;
    heap->data[i] = value;
    heap->size++;
    
    // Fix the min heap property if violated (sift-up)
    while (i > 0 && heap->data[parent(i)] > heap->data[i]) {
        swap(&heap->data[i], &heap->data[parent(i)]);
        i = parent(i);
    }
}

// Extract the minimum element from the heap
int extract_min(MinHeap* heap) {
    if (heap->size <= 0)
        return INT_MAX; // Heap is empty
        
    if (heap->size == 1) {
        heap->size--;
        return heap->data[0];
    }
    
    // Store the minimum value and remove it
    int root = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    
    // Restore the heap property
    heapify(heap, 0);
    
    return root;
}

// Free the heap memory
void free_heap(MinHeap* heap) {
    if (heap) {
        free(heap->data);
        free(heap);
    }
}

// Push operation: increment a value and sift down
int push(MinHeap* heap, int index, int incr, int depth) {
    if (index >= heap->size) return depth;
    
    // Increment the value
    heap->data[index] += incr;
    
    // Sift down if needed
    int smallest = index;
    int left = left_child(index);
    int right = right_child(index);
    
    if (left < heap->size && heap->data[left] < heap->data[smallest])
        smallest = left;
        
    if (right < heap->size && heap->data[right] < heap->data[smallest])
        smallest = right;
        
    if (smallest != index) {
        swap(&heap->data[index], &heap->data[smallest]);
        return push(heap, smallest, 0, depth + 1);
    }
    
    return depth;
}

// Print the heap (for debugging)
void print_heap(MinHeap* heap) {
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->data[i]);
    }
    printf("\n");
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

        for (int iter = 0; iter < num_iterations; iter++) {
            int *array = unsorted_array(size);
            MinHeap* heap = create_heap(size);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                // Insert
                insert(heap, array[i]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long add_time = micro_seconds(&start, &end);
            add_min_time = (add_time < add_min_time) ? add_time : add_min_time;
            add_total_time += add_time;

            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                // Remove min
                extract_min(heap);
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

    benchmark(1000, 128000, 50);
    
    return 0;
}