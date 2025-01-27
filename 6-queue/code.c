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
    return q;
}

int empty(queue *q) {
    return ...;
}

void enque(queue* q, int v) {
    node *nd = (node*)malloc(sizeof(node));
    nd->value = v;
    nd->next = NULL;

    
    // node *prv = NULL;
    // node *nxt = q->first;
    // while (nxt != NULL) {
    //     prv = nxt;
    //     nxt = nxt->next;
    // }
    // if (prv != NULL) {
    //     prv->next = nd;
    // } else {
    //     q->first = nd;
    // }
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
r