#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

typedef struct node {
    int value;
    struct node *right;
    struct node *left;
    } node;

typedef struct tree {
    node *root;
} tree;

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

node *construct_node(int val) { // Corrected return type
    node *nd = (node*)malloc(sizeof(node));
    nd->value = val;
    nd->left = NULL; 
    nd->right = NULL; 
    return nd;
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

void add_recursive(node *now, int val) {
    if (val < now->value) {
        if (now->left == NULL) {
            now->left = construct_node(val);
        } else {
            add_recursive(now->left, val);
        }
    } else if (val > now->value) { 
        if (now->right == NULL) {
            now->right = construct_node(val);
        } else {
            add_recursive(now->right, val);
        }
    } else { 
        return;
    }
}


void add(tree *tr, int val) {
    if (tr->root == NULL) {
        tr->root = construct_node(val);
    } else {
        add_recursive(tr->root, val);
    }
}

bool lookup_while(tree *tr, int val) {
    node *now = tr->root;
    while (now != NULL) {
        if (now->value == val) {
            return true;
        } else if (val < now->value) {
            now = now->left;
        } else {
            now = now->right;
        }
    }
    return false;
} 

bool lookup_recursive(node *now, int val) {
    if (now == NULL) {
        return false;
    } else if (now->value == val) {
        return true;
    } else if (val < now->value) {
        return lookup_recursive(now->left, val);
    } else {
        return lookup_recursive(now->right, val);
    }
}

bool lookup(tree *tr, int val) {
    if (tr->root == NULL) {
        return false;
    }
    return lookup_recursive(tr->root, val);
}

static void print_recursive_node(node *nd) {
    if (nd != NULL) {
        print_recursive_node(nd->left); 
        // printf("%d ", nd->value);
        print_recursive_node(nd->right); 
    }
}

void print_tree(tree *tr) {
    if (tr->root != NULL) {
        print_recursive_node(tr->root);
    }
}

typedef struct cell { // Corrected Stack Data Structure using cell and stack
    struct node *data;
    struct cell *next;
} cell;

typedef struct stack {
    struct cell *first; // Corrected: stack points to the first cell
} stack;

stack *create_stack() { // Corrected: create_stack creates an EMPTY stack
    stack *stk = (stack*)malloc(sizeof(stack));
    if (stk == NULL) { // Handle malloc failure
        fprintf(stderr, "Memory allocation failed in create_stack\n");
        exit(EXIT_FAILURE);
    }
    stk->first = NULL; // Initialize as empty stack
    return stk;
}

void push(stack *stk, node *item) { // Corrected: push function (void return type)
    if (stk == NULL) { // Added NULL check for stack pointer itself (defensive programming)
        fprintf(stderr, "Error: push called with NULL stack pointer\n");
        return; // Or handle error as appropriate
    }
    cell *tmp = (cell*)malloc(sizeof(cell));
    if (tmp == NULL) { // Handle malloc failure
        fprintf(stderr, "Memory allocation failed in push\n");
        exit(EXIT_FAILURE);
    }
    tmp->data = item;
    tmp->next = stk->first;
    stk->first = tmp;
}

node *pop(stack *stk) { // Corrected: pop function returns node*
    if (stk == NULL || stk->first == NULL) { // Check for NULL stack or empty stack
        return NULL; // Stack is empty
    } else {
        cell *tmp = stk->first;
        node *node_data = tmp->data; // Get the node data
        stk->first = tmp->next; // Update stack's first pointer
        free(tmp);
        return node_data; // Return the node pointer
    }
}


bool is_empty_stack(stack *stk) {
    return (stk == NULL || stk->first == NULL); // Corrected: Check if stack pointer is NULL OR if first cell is NULL
}


void print_tree_explicit_stack(tree *tr) {
    stack *stk = create_stack(); // Corrected: Create empty stack
    node *cur = tr->root;

    while (cur != NULL || !is_empty_stack(stk)) {
        while (cur != NULL) {
            push(stk, cur); // Push current node onto stack
            cur = cur->left;  // Go to left child
        }

        node *popped_node = pop(stk); // Pop from stack to get back to a node
        if (popped_node != NULL) {      // Check if pop returned a valid node
            //printf("%d ", popped_node->value); // Print the value of the popped node (In-order visit)
            cur = popped_node->right;     // Move to the right child of the popped node
        } else {
            cur = NULL; // Stack is empty, no more nodes to process in this path.
        }
    }
    free(stk); // Free the stack    
}

