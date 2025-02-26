#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct node {
    int value;
    struct node *next;
} node;

typedef struct queue {
    node *first;
} queue;

// Helper function to create unsorted array
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

// Add destroy queue function
void destroy_queue(queue *q) {
    if (q != NULL) {
        while (q->first != NULL) {
            node *temp = q->first;
            q->first = q->first->next;
            free(temp);
        }
        free(q);
    }
}


queue *create_queue() {
    queue *q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    return q;
}

int empty(queue *q) {
    return (q->first == NULL);
}

void enque(queue* q, int v) {
    node *nd = (node*)malloc(sizeof(node));
    nd->value = v;
    nd->next = NULL;

    node *nxt = q->first;
    if (nxt == NULL) {
        q->first = nd;
        return;
    }
    while (nxt->next != NULL) {
        nxt = nxt->next;
    }
    nxt->next = nd;
}
int dequeue(queue *q) {
    int res = 0;
    if (q->first != NULL) {
        node *nd = q->first;
        res = nd->value;
        q->first = nd->next;
        free(nd);
    }
    return res;
}

void bench_queue(int min_size, int max_size) {
    printf("Benchmarking queue operations from size %d to %d\n", min_size, max_size);
    printf("Size\tMin(ns)\tMax(ns)\tAvg(ns)\tAvg/op(ns)\n");

    // For each size
    for (int size = min_size; size <= max_size; size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;
        
        // Run 50 iterations for each size
        for (int iter = 0; iter < 50; iter++) {
            // Create queue and unsorted array
            queue *q = create_queue();
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
               (double)min_time,        // Total minimum time
               (double)max_time,        // Total maximum time
               (double)total_time / 50, // Average total time
               (double)total_time / (50 * size)); // Average time per operation
    }
}

int main() {
    srand(time(NULL));  // Initialize random seed
    
    // Benchmark with sizes from 1000 to 128000, 50 iterations each
    bench_queue(1000, 128000);
    return 0;
}