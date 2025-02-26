#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int *list;
    struct node *next;
} node;

typedef struct queue {
    int arraySize;
    node *first;
    node *last;
    int firstIndex;
    int lastIndex;
} queue;

queue *create_queue(int arraySize) {
    queue *q = (queue *)malloc(sizeof(queue));
    node *nd = (node *)malloc(sizeof(node));
    int *list = (int *)malloc(arraySize * sizeof(int));
    nd->list = list;
    nd->next = NULL;
    q->arraySize = arraySize;
    q->first = nd;
    q->last = nd;
    q->firstIndex = 0;
    q->lastIndex = 0;
    return q;
}

int empty(queue *q) {
    return (q->first == q->last && q->firstIndex == q->lastIndex);
}

void enque(queue *q, int v) {
    if (q->lastIndex >= q->arraySize) {
        extendQueue(q);
    }
    q->last->list[q->lastIndex] = v;
    q->lastIndex++;
}

void extendQueue(queue *q) {
    node *nd = (node *)malloc(sizeof(node));
    int *list = (int *)malloc(q->arraySize * sizeof(int));
    nd->list = list;
    nd->next = NULL;
    q->last->next = nd;
    q->last = nd;
    q->lastIndex = 0;
}

int dequeue(queue *q) {
    int res = 0;
    if (q->first != NULL) {
        if (q->firstIndex >= q->arraySize) {
            node *nd = q->first;
            q->first = nd->next;
            q->firstIndex = 0;
            free(nd->list);
            free(nd);
        }
        res = q->first->list[q->firstIndex];
        q->firstIndex++;
    }
    return res;
}