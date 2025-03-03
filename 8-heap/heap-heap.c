#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

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


// Heap Node structure
typedef struct Node {
    int value;              // Value stored at this node
    int size;               // Size of the subtree rooted at this node
    struct Node *left;      // Left child
    struct Node *right;     // Right child
} Node;

// Helper function to create a new node
Node* create_node(int value) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->size = 1;   // New node's size is 1 (itself)
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Function to update the size of the node
void update_size(Node* node) {
    if (node == NULL) return;
    node->size = 1 + (node->left ? node->left->size : 0) + (node->right ? node->right->size : 0);
}

// Insert an element into the heap, balancing by the subtree size
Node* insert(Node* root, int value) {
    if (root == NULL) {
        return create_node(value);
    }

    // Balance insertion: insert into the subtree with fewer elements
    if (root->left == NULL || (root->right && root->left->size > root->right->size)) {
        root->left = insert(root->left, value);
    } else {
        root->right = insert(root->right, value);
    }

    // After insertion, check and maintain the heap property by ensuring the heap order
    if (root->left && root->left->value < root->value) {
        // Swap root with left child if left child has a smaller value
        int temp = root->value;
        root->value = root->left->value;
        root->left->value = temp;
    }
    if (root->right && root->right->value < root->value) {
        // Swap root with right child if right child has a smaller value
        int temp = root->value;
        root->value = root->right->value;
        root->right->value = temp;
    }

    // Update the size of the current root
    update_size(root);
    return root;
}

// Remove the root element (min element) and re-balance the heap
Node* remove_min(Node* root) {
    if (root == NULL) return NULL;

    // If the heap has only one element
    if (root->left == NULL && root->right == NULL) {
        free(root);
        return NULL;
    }

    // Choose the child with the smallest value to promote to root
    Node *child;
    if (root->left == NULL) {
        child = root->right;
    } else if (root->right == NULL) {
        child = root->left;
    } else {
        child = (root->left->value < root->right->value) ? root->left : root->right;
    }

    // Promote the smallest child to the root and recursively remove the smallest element from the child
    root->value = child->value;
    if (child == root->left) {
        root->left = remove_min(root->left);
    } else {
        root->right = remove_min(root->right);
    }

    // Update the size after removal
    update_size(root);
    return root;
}

// Push operation: Increment the root element and push it down the tree
int push(Node* root, int incr, int depth) {
    if (root == NULL) return depth;

    // Increment the root element
    root->value += incr;

    // If the root is greater than either of its children, push it down
    if (root->left && root->left->value < root->value) {
        // Swap with the left child and recursively push down
        int temp = root->value;
        root->value = root->left->value;
        root->left->value = temp;
        return push(root->left, incr, depth + 1);
    } else if (root->right && root->right->value < root->value) {
        // Swap with the right child and recursively push down
        int temp = root->value;
        root->value = root->right->value;
        root->right->value = temp;
        return push(root->right, incr, depth + 1);
    }

    // No further push needed
    return depth;
}

// Helper function to print the tree (In-order traversal)
void print_tree(Node* root) {
    if (root == NULL) return;
    print_tree(root->left);
    printf("%d ", root->value);
    print_tree(root->right);
}

// Free the tree nodes to avoid memory leak
void free_tree(Node* root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}



// Benchmark adding elements to the heap
void benchmark(int min_size, int max_size, int num_iterations) {
    printf("Benchmarking Add and remove Operations\n");
    printf("Array size      Add(min,µs)     Add(avg/µs)      Rmv(min,µs)     Rmv(avg/µs)     \n");

    for (int size = min_size; size <= max_size; size *= 2) {
        long add_min_time = LONG_MAX;
        long add_total_time = 0;
        long rmv_min_time = LONG_MAX;
        long rmv_total_time = 0;

    Node* root = NULL;

        for (int iter = 0; iter < num_iterations; iter++) {
            int *array = unsorted_array(size);

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                root = insert(root, array[i]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long add_time = micro_seconds(&start, &end);
            add_min_time = (add_time < add_min_time) ? add_time : add_min_time;
            add_total_time += add_time;

            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < size; i++) {
                /// Insert
                remove_min(root);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            long rmv_time = micro_seconds(&start, &end);
            rmv_min_time = (rmv_time < rmv_min_time) ? rmv_time : rmv_min_time;
            rmv_total_time += rmv_time;

            free(array);
        }

        free_tree(root);

        printf("%-11d       %-8.0f      %-8.0f     %-8.0f      %-8.0f\n", 
            size, 
            (double)add_min_time,
            (double)add_total_time / num_iterations,
            (double)add_min_time,
            (double)add_total_time / num_iterations
        );
    }
}


int main() {
    srand(time(NULL));

    benchmark(1000, 128000, 50);
    
    return 0;
}


// int main() {
//     Node* root = NULL;

//     // Inserting elements
//     root = insert(root, 10);
//     root = insert(root, 5);
//     root = insert(root, 8);
//     root = insert(root, 3);
//     root = insert(root, 12);
//     root = insert(root, 7);

//     printf("Heap after insertions: ");
//     print_tree(root);
//     printf("\n");

//     // Remove the min element
//     root = remove_min(root);
//     printf("Heap after removing the min element: ");
//     print_tree(root);
//     printf("\n");

//     // Push the root element down with increment
//     int depth = push(root, 6, 0);
//     printf("Heap after pushing root element down: ");
//     print_tree(root);
//     printf("\n");
//     printf("Depth pushed down: %d\n", depth);

//     // Free the heap tree
//     free_tree(root);

//     return 0;
// }
