#include <stdio.h>
#include <stdlib.h>

typedef struct queue {
    int arraySize;
    int *list;
    int firstIndex;
    int lastIndex;
    int count;        
} queue;

queue *create_queue(int arraySize) {
    queue *q = (queue*)malloc(sizeof(queue));
    int *list = (int *)malloc(arraySize * sizeof(int));
    q->arraySize = arraySize;
    q->list = list;
    q->firstIndex = 0;
    q->lastIndex = 0;
    q->count = 0;
    return q;
}

int empty(queue *q) {
    return (q->count == 0); 
}

void enque(queue* q, int v) {
    if (q->count == q->arraySize) {
        extendQueue(q);
    }
    
    q->list[q->lastIndex] = v;
    q->lastIndex = (q->lastIndex + 1) % q->arraySize; 
    q->count++;
}

void extendQueue(queue *q) {
    int *biggerList = (int *)malloc(q->arraySize * 2 * sizeof(int));
    
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

int dequeue(queue *q) {
    int res = 0;
    if (q->count > 0) { 
        res = q->list[q->firstIndex];
        q->firstIndex++;
        q->count--;
        if (q->firstIndex >= q->arraySize) {
            q->firstIndex = 0;
        }
    }
    return res;
}

