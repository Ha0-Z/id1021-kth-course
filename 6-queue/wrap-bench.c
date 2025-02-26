#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

int *create_unsorted_array(int size) {
    int *arr = (int*)malloc(size * sizeof(int));
    if (arr == NULL) return NULL;
    
    // Fill with sequential numbers
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    
    // Shuffle the array
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    return arr;
}

// Helper function for time measurement
long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}

typedef struct queue {
    int arraySize;
    int *list;
    int firstIndex;
    int lastIndex;
    int count;
} queue;

queue *create_queue(int arraySize) {
    queue *q = (queue*)malloc(sizeof(queue));
    if (q == NULL) return NULL;
    
    int *list = (int *)malloc(arraySize * sizeof(int));
    if (list == NULL) {
        free(q);
        return NULL;
    }
    
    q->arraySize = arraySize;
    q->list = list;
    q->firstIndex = 0;
    q->lastIndex = 0;
    q->count = 0;
    return q;
}

void destroy_queue(queue *q) {
    if (q != NULL) {
        if (q->list != NULL) {
            free(q->list);
        }
        free(q);
    }
}

int empty(queue *q) {
    return (q->count == 0);
}

void extendQueue(queue *q) {
    int *biggerList = (int *)malloc(q->arraySize * 2 * sizeof(int));
    if (biggerList == NULL) return;  // Handle error in production code
    
    for (int i = 0; i < q->count; i++) {
        int oldIndex = (q->firstIndex + i) % q->arraySize;
        biggerList[i] = q->list[oldIndex];
    }
    
    free(q->list);
    q->list = biggerList;
    q->firstIndex = 0;
    q->lastIndex = q->count;
    q->arraySize = q->arraySize * 2;
}

void enque(queue* q, int v) {
    if (q->count == q->arraySize) {
        extendQueue(q);
    }
    
    q->list[q->lastIndex] = v;
    q->lastIndex = (q->lastIndex + 1) % q->arraySize;
    q->count++;
}

int dequeue(queue *q) {
    int res = 0;
    if (q->count > 0) {
        res = q->list[q->firstIndex];
        q->firstIndex = (q->firstIndex + 1) % q->arraySize;
        q->count--;
    }
    return res;
}

void bench_queue(int min_size, int max_size) {
    printf("Benchmarking queue operations from size %d to %d\n", min_size, max_size);
    printf("Size\tMin(ns)\tMax(ns)\tAvg(ns)\tAvg/op(ns)\n");  // Added the last column header

    // For each size
    for (int size = min_size; size <= max_size; size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;
        
        // Run 50 iterations for each size
        for (int iter = 0; iter < 50; iter++) {
            // Create queue and unsorted array
            queue *q = create_queue(8);  // Start small to test extension
            int *test_array = create_unsorted_array(size);
            
            // Start timing
            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            
            // Enqueue all items
            for (int i = 0; i < size; i++) {
                enque(q, test_array[i]);
            }
            
            // Dequeue all items
            for (int i = 0; i < size; i++) {
                dequeue(q);
            }
            
            // Stop timing
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            
            // Calculate time
            long wall_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000 +
                           (end_time.tv_nsec - start_time.tv_nsec);
            
            // Update statistics
            min_time = (wall_time < min_time) ? wall_time : min_time;
            max_time = (wall_time > max_time) ? wall_time : max_time;
            total_time += wall_time;
            
            // Cleanup
            destroy_queue(q);
            free(test_array);
        }
        
        // Print results for this size
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n", 
               size, 
               (double)min_time,   // Time per operation
               (double)max_time,
               (double)total_time / 50,
               (double)total_time / (50 * size));  // Average over 50 iterations
    }
}


int main() {
    srand(time(NULL));  // Initialize random seed
    
    // Benchmark with sizes from 1000 to 128000, 50 iterations each
    bench_queue(1000, 128000);
    return 0;
}
