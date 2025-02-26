#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Your queue implementation remains the same...

typedef struct node {
    int value;
    struct node *next;
} node;

typedef struct queue {
    node *first;
    node *last;
} queue;


queue *create_queue() {
    queue *q = (queue*)malloc(sizeof(queue));
    q->first = NULL;
    q->last = NULL;
    return q;
}

int empty(queue *q) {
    return (q->first == NULL);
}

void enque(queue* q, int v) {
    node *nd = (node*)malloc(sizeof(node));
    nd->value = v;
    nd->next = NULL;

    
    if (q->first == NULL) {
        q->first = nd;
        q->last = nd;
        return;
    }
    q->last->next = nd;
    q->last = nd;
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

// Helper function to create unsorted array
int *create_unsorted_array(int size) {
    int *arr = (int*)malloc(size * sizeof(int));
    if (arr == NULL) return NULL;
    
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    return arr;
}



void bench_queue(int min_size, int max_size) {
    printf("Benchmarking queue operations from size %d to %d\n", min_size, max_size);
    printf("Size\tMin(ns)\tMax(ns)\tAvg(ns)\tAvg/op(ns)\n");

    for (int size = min_size; size <= max_size; size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;
        
        for (int iter = 0; iter < 50; iter++) {
            queue *q = create_queue();
            int *test_array = create_unsorted_array(size);
            
            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            
            for (int i = 0; i < size; i++) {
                enque(q, test_array[i]);
            }
            
            for (int i = 0; i < size; i++) {
                dequeue(q);
            }
            
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            
            long wall_time = (end_time.tv_sec - start_time.tv_sec) * 1000000000 +
                           (end_time.tv_nsec - start_time.tv_nsec);
            
            min_time = (wall_time < min_time) ? wall_time : min_time;
            max_time = (wall_time > max_time) ? wall_time : max_time;
            total_time += wall_time;
            
            destroy_queue(q);
            free(test_array);
        }
        
        printf("%d\t%.2f\t%.2f\t%.2f\t%.2f\n",
               size,
               (double)min_time,
               (double)max_time,
               (double)total_time / 50,
               (double)total_time / (50 * size));
    }
}

int main() {
    srand(time(NULL));
    bench_queue(1000, 128000);
    return 0;
}