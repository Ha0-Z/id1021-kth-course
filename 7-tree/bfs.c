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
    struct node *next;
    } node;

typedef struct bfs {
    node *first;
    node *last;
    node *current;
} bfs;

bfs *construct_bfs() { 
    bfs *this = (bfs*)malloc(sizeof(bfs));
    this->first = NULL;
    this->last = NULL;
    this->current = NULL;
    return this;
}

node *construct_empty() {
    node *new = (node*)malloc(sizeof(node));
    new->value = 0;
    new->left = NULL;
    new->right = NULL;
    new->next = NULL;
    return new;
}

void insert(bfs *tree, int val){
    node *left = construct_empty();
    node *right = construct_empty();
    left->next = right;

    if (tree == NULL) {
        node *new = construct_empty();
        new->value = val;
        new->left = left;
        new->right = right;
        new->next = left;
        
        tree->first = new;
        tree->last = right;
        tree->current = left;
    } else {
        node *now = tree->current;
        now->value = val;
        now->left = left;
        now->right = right;
        
        tree->current = now->next;
        tree->last = right;
    }
}

