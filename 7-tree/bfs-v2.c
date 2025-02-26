#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

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

typedef struct node {
    int value;
    struct node *right;
    struct node *left;
    } node;

typedef struct tree {
    node *root;
} tree;

typedef struct cell { 
    int data;
    struct cell *next;
    struct node *node;
} cell;

typedef struct queue {
    struct cell *first;
    struct cell *last;
} queue;

queue *create_queue() { 
    queue *q = (queue*)malloc(sizeof(queue));

    q->first = NULL; 
    q->last = NULL;
    return q;
}

typedef struct sequence {
    queue *q;
} sequence;


tree *construct_tree() { 
    tree *tr = (tree*)malloc(sizeof(tree));
    tr->root = NULL;
    return tr;
}

void free_node(node *nd) {
    if (nd != NULL) {
        free_node(nd->left);
        free_node(nd->right);
        free(nd);
    }
}

void free_tree(tree *tr) {
    if (tr != NULL) {
        free_node(tr->root);
        free(tr);
    }
}

node *construct_node(int val) {
    node *nd = (node*)malloc(sizeof(node));
    if (nd == NULL) {
        fprintf(stderr, "Memory allocation failed in construct_node\n");
        exit(EXIT_FAILURE);
    }
    nd->value = val;
    nd->left = NULL; 
    nd->right = NULL; 
    return nd;
}

void enqueue(queue *q, node *nd) {
    cell *tmp = (cell*)malloc(sizeof(cell));
    tmp->data = nd->value;
    tmp->next = NULL;
    tmp->node = nd;

    if (q->first == NULL) {
        q->first = tmp;
        q->last = tmp;
    } else {
        q->last->next = tmp;
        q->last = tmp;
    }
}

cell* dequeue(queue *q) {
    if (q == NULL || q->first == NULL) {
        return NULL;
    }

    cell *tmp = q->first;
    q->first = q->first->next;
    
    // Queue is now empty
    if (q->first == NULL) {
        q->last = NULL; 
    }
    return tmp;
}

bool is_empty(queue *q) {
    return (q == NULL || q->first == NULL);
}

void free_queue(queue *q) {
    if (q == NULL) {
        return;
    }
    
    cell *current = q->first;
    while (current != NULL) {
        cell *next = current->next;
        free(current);
        current = next;
    }
    
    free(q);
}

void bfs(tree *tr) {
    if (tr == NULL || tr->root == NULL) {
        return;
    }
    
    queue *q = create_queue();
    enqueue(q, tr->root);
    
    while (!is_empty(q)) {
        cell *current = dequeue(q);
        //printf("%d ", current->data);  // Process the current node
        
        // Add children to the queue
        if (current->node->left != NULL) {
            enqueue(q, current->node->left);
        }
        if (current->node->right != NULL) {
            enqueue(q, current->node->right);
        }
        
        free(current);  // Free the dequeued cell
    }
    
    free_queue(q);
}

void read_tree_bfs(tree *tr) {
    if (tr == NULL || tr->root == NULL) {
        return;
    }
    
    queue *q = create_queue();
    enqueue(q, tr->root);
    
    //printf("BFS traversal: ");
    while (!is_empty(q)) {
        cell *current = dequeue(q);
        //printf("%d ", current->data);
        
        if (current->node->left != NULL) {
            enqueue(q, current->node->left);
        }
        if (current->node->right != NULL) {
            enqueue(q, current->node->right);
        }
        
        free(current);
    }
    
    free_queue(q);
}


sequence* create_sequence(tree *tr) {
    sequence *seq = (sequence*)malloc(sizeof(sequence));
    if (seq == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    seq->q = create_queue();
    if (tr != NULL && tr->root != NULL) {
        enqueue(seq->q, tr->root);
    }
    return seq;
}

int next(sequence *seq) {
    if (seq == NULL || is_empty(seq->q)) {
        return INT_MIN;  // Signal end of sequence
    }
    
    cell *current = dequeue(seq->q);
    int value = current->data;
    
    if (current->node->left != NULL) {
        enqueue(seq->q, current->node->left);
    }
    if (current->node->right != NULL) {
        enqueue(seq->q, current->node->right);
    }
    
    free(current);
    return value;
}

bool has_next(sequence *seq) {
    return (seq != NULL && !is_empty(seq->q));
}

void free_sequence(sequence *seq) {
    if (seq != NULL) {
        free_queue(seq->q);
        free(seq);
    }
}



int *sorted_array(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
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

void add_while(tree *tr, int val) {
    if (tr->root == NULL) {
        tr->root = construct_node(val);
    } else {
        node *now = tr->root;
        while (true) {
            if (val < now->value) {
                if (now->left == NULL) {
                    now->left = construct_node(val);
                    return;
                } else {
                    now = now->left;
                }
            } else if (val > now->value) { 
                if (now->right == NULL) {
                    now->right = construct_node(val);
                    return;
                } else {
                    now = now->right;
                }
            } else { 
                return;
            }
        }
    }
}

tree *create_test_tree(int size) {
    int *test_array = unsorted_array(size);
    tree *test_tree = construct_tree();
    for (int i = 0; i < size; i++) {
        add_while(test_tree, test_array[i]); // Using add_while for tree construction in print benchmark for consistency, can be changed.
    }
    free(test_array);
    return test_tree;
}

int count_nodes(node *nd) {
    if (nd == NULL) {
        return 0;
    }
    return 1 + count_nodes(nd->left) + count_nodes(nd->right);
}

void bench_bfs(int min_tree_size, int max_tree_size, int num_iterations) {
    printf("--- Benchmarking BFS Traversal ---\n");
    printf("Tree size   TimeMin (µs)   TimeMax (µs)   Average (µs)\n");

    for (int current_size = min_tree_size; current_size <= max_tree_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            tree *test_tree = create_test_tree(current_size);
            int total_nodes = count_nodes(test_tree->root); // Count total nodes

            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // ---  Optionally capture output to count printed nodes for verification ---
            // (This is more complex and might not be necessary for timing benchmark)
            // --- Or just rely on correct BFS implementation and total_nodes count ---

            bfs(test_tree); // Call BFS function to benchmark

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = micro_seconds(&start_time, &end_time);

            // --- Verification (compare total_nodes with expected/printed nodes) ---
            // (For simple verification, just print total_nodes for now)

            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;

            free_tree(test_tree);
        }


        printf("%-10d   %-12.2f   %-12.2f   %-12.2f\n",
               current_size,
               (double)min_time,
               (double)max_time,
               (double)total_time / num_iterations);
    }
}

void bench_enqueue_dequeue(int min_queue_size, int max_queue_size, int num_iterations) {
    printf("\n--- Benchmarking Enqueue/Dequeue Operations ---\n");
    printf("Queue size   TimeMin (µs)   TimeMax (µs)   Average (µs)\n");

    for (int current_size = min_queue_size; current_size <= max_queue_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            queue *test_queue = create_queue();
            tree *test_tree = create_test_tree(current_size); // Need nodes to enqueue
            node **nodes_to_enqueue = malloc(current_size * sizeof(node*)); // Array to hold nodes

            // Collect nodes from tree for enqueuing (using BFS for simplicity)
            int node_count = 0;
            queue *temp_queue = create_queue();
            enqueue(temp_queue, test_tree->root);
            while (!is_empty(temp_queue) && node_count < current_size) {
                cell *current_cell = dequeue(temp_queue);
                if (current_cell != NULL && current_cell->node != NULL) { // Check for NULL node
                    nodes_to_enqueue[node_count++] = current_cell->node;
                    if (current_cell->node->left != NULL) enqueue(temp_queue, current_cell->node->left);
                    if (current_cell->node->right != NULL) enqueue(temp_queue, current_cell->node->right);
                }
                free(current_cell);
            }
            free_queue(temp_queue);


            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // Benchmark Enqueue and Dequeue
            for (int i = 0; i < current_size; ++i) {
                enqueue(test_queue, nodes_to_enqueue[i]);
            }
            for (int i = 0; i < current_size; ++i) {
                cell *dequeued_cell = dequeue(test_queue);
                free(dequeued_cell); // Free dequeued cell
            }


            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = micro_seconds(&start_time, &end_time);

            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;

            free_tree(test_tree);
            free_queue(test_queue);
            free(nodes_to_enqueue);
        }

        printf("%-10d   %-12.2f   %-12.2f   %-12.2f\n",
               current_size,
               (double)min_time,
               (double)max_time,
               (double)total_time / num_iterations);
    }
}


int main() {
    srand(time(NULL)); // Seed random number generator

    bench_bfs(1000, 128000, 50); // Benchmark BFS traversal
    bench_enqueue_dequeue(1000, 128000, 50); // Benchmark enqueue and dequeue

    return 0;
}