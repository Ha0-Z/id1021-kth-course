#include <stdio.h>
#include <stdlib.h> 

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
