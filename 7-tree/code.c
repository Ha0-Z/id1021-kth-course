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

void free_tree(tree *tr) {
    if (tr != NULL) {
        free_node(tr->root);
        free(tr);
    }
}

node *construct_node(int val) {
    node *nd = (node*)malloc(sizeof(node));
    nd->value = val;
    nd->left = null;
    nd->right = null;
    return nd;
}

void free_node(node *nd) {
    if (nd != NULL) {
        free_node(nd->left);
        free_node(nd->right);
        free(nd);
    }
}

void add_while(tree *tr, int val) {
    if (tr->root == NULL) {
        tr->root = construct_node(val);
    } else {
        node *now = tr->root;
        while (true)
        {
            if (val < now->value) {
                if (now->left == NULL) {
                    now->left = construct_node(val);
                    return;
                } else {
                    now = now->left;
                }
            } else {
                if (now->right == NULL) {
                    now->right = construct_node(val);
                    return;
                } else {
                    now = now->right;
                }
            }
        }        
    }
}

void add(tree *tr, int val) {
    if (tr->root == NULL) {
        tr->root = construct_node(val);
    } else {
        add_recursive(tr->root, val);
    }
}
void add_recursive(node *now, int val) {
    if (val < now->value) {
        if (now->left == NULL) {
            now->left = construct_node(val);
        } else {
            add_recursive(now->left, val);
        }
    } else {
        if (now->right == NULL) {
            now->right = construct_node(val);
        } else {
            add_recursive(now->right, val);
        }
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

bool lookup(tree *tr, int val) {
    return lookup_recursive(tr->root, val);
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

static void print(node *nd) {
    if (nd != NULL) {
    print(nd->left);
    printf("%d ", nd->value);
    print(nd->right);
    }

}

void print_tree(tree *tr) {
    if (tr->root != NULL) {
        print(tr->root);
    }
    printf("\n");
}

typedef struct stack {
    node *data;
    struct stack *next;
} stack;

stack *create_stack() {
    return NULL;
}

void push(stack **stk, node *nd) {
    stack *new = (stack*)malloc(sizeof(stack));
    new->data = nd;
    new->next = *stk;
    *stk = new;
}

node *pop(stack **stk) {
    if (*stk == NULL) {
        return NULL;
    } else {
        stack *tmp = *stk;
        node *nd = tmp->data;
        *stk = tmp->next;
        free(tmp);
        return nd;
    }
}

void print(tree *tr) {
    stack *stk = create_stack();
    node *cur = tr->root;
    // move to the leftmost node
    while(cur != NULL) {
        // print value of node
        printf("%d ", cur->value);
        if( cur->right != NULL) {
            // move to the leftmost node, push nodes as you go
            push(&stk, cur->right);
        } else {
            // pop a node from the stack
            printf("%d ", pop(&stk)->value);
        }
        
    }
}