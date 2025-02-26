#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

// Corrected time difference calculation (handles negative nanoseconds)
long micro_seconds(struct timespec *start, struct timespec *end) {
    long sec_diff = end->tv_sec - start->tv_sec;
    long nsec_diff = end->tv_nsec - start->tv_nsec;
    
    if (nsec_diff < 0) {
        sec_diff--;
        nsec_diff += 1000000000;
    }
    return (sec_diff * 1000000) + (nsec_diff / 1000);
}



typedef struct cell {
    int value;
    struct cell *tail;
} cell;

typedef struct linked {
    cell *first;
} linked;

linked *linked_create() {
    linked *new = (linked*)malloc(sizeof(linked));
    new->first = NULL;
    return new;
}

void linked_free(linked *lnk) {
    cell *nxt = lnk->first;
    while (nxt != NULL) {
        cell *tmp = nxt->tail;
        free(nxt);
        nxt = tmp;
    }
    free(lnk);
}

void linked_add(linked *lnk, int item) {
    cell *new = (cell*)malloc(sizeof(cell));
    new->value = item;
    new->tail = lnk->first;  
    lnk->first = new;      
}

linked *linked_init(int n) {
    linked *a = linked_create();
    for (int i = 0; i < n; i++) {
        linked_add(a, rand() % 10 + i);
    }
    return a;
}

int linked_length(linked *lnk){
    int length = 0;
    cell *nxt = lnk->first;
    while (nxt != NULL) {
        nxt = nxt->tail;
        length++;
    }
    return length;
}

bool linked_find(linked *lnk, int item){
    cell *nxt = lnk->first;
    while (nxt != NULL) {
        if (nxt->value == item){
            return true;
        }
        nxt = nxt->tail;
    }
    return false;
}

void linked_remove(linked *lnk, int item){
    cell *nxt = lnk->first;
    cell *prv = NULL;
    while (nxt != NULL) {
        if (nxt->value == item){
            if (prv == NULL){
                lnk->first = nxt->tail;
            }else {
                prv->tail = nxt->tail;
            }
            free(nxt);
            return;
        }
        prv = nxt;
        nxt = nxt->tail;
    }
}

void linked_append(linked *a, linked *b) {
    // B is empty
    if (b->first == NULL) {
        return;
    }  
    // A is empty
    if (a->first == NULL) {
        a->first = b->first;
        b->first = NULL;
        return;
    }
    cell *now = a->first;
    while(now->tail != NULL) {
        now = now->tail;
    }
    now->tail = b->first;
    b->first = NULL;
}


// Fixed benchmark function
void bench(int min_array_size, int max_array_size, int num_iterations) {
    printf("Running appends on lists from size %d to %d\n", min_array_size, max_array_size);
    printf("Array size   Min (µs)   Max (µs)   Avg (µs)\n");

    for (int current_size = min_array_size; current_size <= max_array_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // Setup
            linked *testlist = linked_init(current_size);
            linked *targetlist = linked_init(1000);

            // Timing
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            linked_append(testlist, targetlist);
            
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            // Cleanup
            linked_free(testlist);
            linked_free(targetlist);
            
            // Calculate iteration time
            long iter_time = micro_seconds(&start, &end);
            
            // Update statistics
            min_time = (iter_time < min_time) ? iter_time : min_time;
            max_time = (iter_time > max_time) ? iter_time : max_time;
            total_time += iter_time;
        }

        printf("%-11d   %-8.0f   %-8.0f   %-8.0f\n", 
              current_size, 
              (double)min_time,
              (double)max_time,
              (double)total_time / num_iterations);
    }
}

int main() {
    bench(1000, 128000, 50);  // Adjusted max to power-of-2 boundary
    return 0;
}