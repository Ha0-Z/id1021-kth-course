#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>


// --- Data Structures ---
typedef struct node {
    int value;
    struct node *right;
    struct node *left;
} node;

typedef struct tree {
    node *root;
} tree;

typedef struct cell {
    struct node *node; // Store node pointer directly, no need for data duplication
    struct cell *next;
} cell;

typedef struct queue {
    struct cell *first;
    struct cell *last;
} queue;

typedef struct sequence {
    queue *q;
} sequence;

// --- Array Creation for Tree Testing (No changes needed) ---
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

// --- Queue Operations ---
queue *create_queue() {
    queue *q = (queue*)malloc(sizeof(queue));
    if (q == NULL) {
        fprintf(stderr, "Memory allocation failed in create_queue\n");
        exit(EXIT_FAILURE);
    }
    q->first = NULL;
    q->last = NULL;
    return q;
}

void enqueue(queue *q, node *nd) {
    if (q == NULL || nd == NULL) return; // Added null checks for robustness

    cell *tmp = (cell*)malloc(sizeof(cell));
    if (tmp == NULL) {
        fprintf(stderr, "Memory allocation failed in enqueue\n");
        exit(EXIT_FAILURE);
    }
    tmp->node = nd; // Store the node pointer directly
    tmp->next = NULL;

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
        return NULL; // Or consider returning a special error value/flag if needed
    }

    cell *tmp = q->first;
    q->first = q->first->next;

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

// --- Tree Operations ---
tree *construct_tree() {
    tree *tr = (tree*)malloc(sizeof(tree));
    if (tr == NULL) {
        fprintf(stderr, "Memory allocation failed in construct_tree\n");
        exit(EXIT_FAILURE);
    }
    tr->root = NULL;
    return tr;
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
                return; // Value already exists, do not add duplicate in this version
            }
        }
    }
}

tree *create_test_tree(int size) {
    int *test_array = unsorted_array(size);
    tree *test_tree = construct_tree();
    for (int i = 0; i < size; i++) {
        add_while(test_tree, test_array[i]);
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


static void print_recursive_node(node *nd) {
    if (nd != NULL) {
        print_recursive_node(nd->left);
        printf("%d ", nd->value);
        print_recursive_node(nd->right);
    }
}

void print_tree(tree *tr) {
    if (tr->root != NULL) {
        print_recursive_node(tr->root);
    }
    printf("\n");
}


// --- Breadth-First Search and Lazy Sequence ---

sequence* create_sequence(tree *tr) {
    sequence *seq = (sequence*)malloc(sizeof(sequence));
    if (seq == NULL) {
        fprintf(stderr, "Memory allocation failed in create_sequence\n");
        exit(EXIT_FAILURE);
    }
    seq->q = create_queue();
    if (tr != NULL && tr->root != NULL) {
        enqueue(seq->q, tr->root); // Initialize queue with the root node
    }
    return seq;
}

int next(sequence *seq) {
    if (seq == NULL || is_empty(seq->q)) {
        return INT_MIN;  // Signal end of sequence, or consider returning a specific error code/flag
    }

    cell *current_cell = dequeue(seq->q); // Dequeue the cell (containing the node)
    node *current_node = current_cell->node; // Get the node from the cell
    int value = current_node->value; // Get the value from the node

    // Enqueue children of the dequeued node (if they exist)
    if (current_node->left != NULL) {
        enqueue(seq->q, current_node->left);
    }
    if (current_node->right != NULL) {
        enqueue(seq->q, current_node->right);
    }

    free(current_cell); // Free the cell, but NOT the node itself
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


// --- BFS Traversal Function (Optional, for demonstration/testing) ---
void bfs_traversal(tree *tr) { // Renamed to bfs_traversal to avoid confusion, and made to print
    if (tr == NULL || tr->root == NULL) {
        return;
    }

    queue *q = create_queue();
    enqueue(q, tr->root);

    printf("BFS Traversal: ");
    while (!is_empty(q)) {
        cell *current_cell = dequeue(q);
        node *current_node = current_cell->node;
        printf("%d ", current_node->value); // Process and print the node's value

        if (current_node->left != NULL) {
            enqueue(q, current_node->left);
        }
        if (current_node->right != NULL) {
            enqueue(q, current_node->right);
        }
        free(current_cell);
    }
    printf("\n");
    free_queue(q);
}


// --- Sequence Data Printing (Non-destructive version) ---
void print_sequence_data(sequence *seq) { // Non-destructive print, for demonstration
    if (seq == NULL) return;

    queue *temp_q = create_queue(); // Create a temporary queue to hold elements while printing
    cell *current_cell;

    printf("Sequence data: ");
    while (has_next(seq)) {
        current_cell = dequeue(seq->q); // Dequeue from the sequence's queue
        if (current_cell != NULL) {
            printf("%d ", current_cell->node->value);
            enqueue(temp_q, current_cell->node); // Enqueue to the temp queue for re-queueing later
            free(current_cell); // Free the dequeued cell
        } else {
            break; // Handle potential null dequeue (though unlikely with has_next check)
        }
    }
    printf("\n");

    // Re-queue elements back to the original sequence's queue
    while (!is_empty(temp_q)) {
        current_cell = dequeue(temp_q);
        if (current_cell != NULL) {
            enqueue(seq->q, current_cell->node); // Re-enqueue back to the original queue
            free(current_cell); // Free the cell from the temp queue
        }
    }
    free_queue(temp_q); // Free the temporary queue
}





// --- Lazy BFS Traversal Print (Demonstration using sequence) ---
void read_tree_bfs_lazy(sequence *seq) {
    if (seq == NULL || is_empty(seq->q)) {
        return;
    }

    printf("Lazy BFS Traversal: ");
    while (has_next(seq)) {
        int value = next(seq);
        printf("%d ", value);
    }
    printf("\n");
}


// --- Main Function for Testing ---
int main() {
    srand(time(NULL));

    tree *test_tree = create_test_tree(15);

    sequence *seq = create_sequence(test_tree);

    printf("Initial tree (in-order traversal):\n");
    print_tree(test_tree);
    printf("Initial BFS sequence data (non-destructive print):\n");
    print_sequence_data(seq); // Non-destructive print

    printf("\nExtracting first 3 values lazily:\n");
    for (int i = 0; i < 3; i++) {
        int value = next(seq);
        printf("Extracted: %d\n", value);
    }

    printf("\nTaking a break and modifying the tree...\n");
    add_while(test_tree, 10);
    add_while(test_tree, 15);
    add_while(test_tree, 30);
    node *left_node_root = test_tree->root->left;
    free_node(left_node_root);
    test_tree->root->left = NULL;

    printf("\nData after modification (in-order traversal):\n");
    print_tree(test_tree);

    printf("Sequence data after break (non-destructive print, should be same as initial):\n");
    print_sequence_data(seq); // Non-destructive print - should still show original sequence

    printf("\nExtracting more values after the break (from original sequence):\n");
    for (int i = 0; i < 5; i++) { // Extract a few more values
        if(has_next(seq)){ // Check if there are more elements to extract
            int value = next(seq);
            printf("Extracted: %d\n", value);
        } else {
            printf("Sequence exhausted.\n");
            break; // Exit loop if no more elements
        }
    }

    printf("\nFull BFS traversal of the *modified* tree:\n");
    bfs_traversal(test_tree); // BFS on the modified tree to show changes

    free_sequence(seq);
    free_tree(test_tree);

    return 0;
}