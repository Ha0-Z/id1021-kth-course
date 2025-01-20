#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct stack
{
    int top;
    int size;
    int *array;
} stack;
stack *new_stack(int size)
{
    int *array = (int *)malloc(size * sizeof(int));
    stack *stk = (stack *)malloc(sizeof(stack));
    
    stk->top = -1; // After push the first element top become 0
    stk->size = size;
    stk->array = array;
 return stk;
}
void push(stack *stk, int val)
{
    if (stk->top == stk->size - 1) {
        int newSize = stk->size * 2;
        int *copy = (int *)malloc(newSize * sizeof(int));
        for (int i = 0; i < (stk->size); i++) {
            copy[i] = stk->array[i];
        }
        free(stk->array);

        stk->size = newSize;
        stk->array = copy;
    }

    // Push val to the top of the stack and increment top by one.
    stk->top = (stk->top + 1);
    stk->array[stk->top] = val;
    
}
int pop(stack *stk)
{
    if (stk->top <= -1)
    {
        printf("Cannot pop a empty stack!\n");
        return -2147483647;
    }
    else
    {
        if (stk-> top < stk->size / 4) {
            int newSize = stk->size / 2;
            int *copy = (int *)malloc(newSize * sizeof(int));
            for (int i = 0; i <= stk->top; i++) {
                copy[i] = stk->array[i];
            }
            free(stk->array);

            stk->size = newSize;
            stk->array = copy;            
        }   
        stk->top = stk->top - 1;
        return stk->array[(stk->top + 1)];
    }
    

}



int main() {
    stack *stk = new_stack(16);
    printf("HP-35 pocket calculator\n");
    size_t n = 100;
    char *buffer = (char *)malloc(n * sizeof(char));
    bool run = true;
    while(run) {
        printf(" > ");
        getline(&buffer, &n, stdin);
        if (strcmp(buffer, "\n") == 0) {
            run = false;
        } else if (strcmp(buffer, "+\n") == 0) {
            int a = pop(stk);
            int b = pop(stk);
        push(stk, a+b);
        }else if (strcmp(buffer, "-\n") == 0) {
            int a = pop(stk);
            int b = pop(stk);
        push(stk, b-a);
        }else if (strcmp(buffer, "*\n") == 0) {
            int a = pop(stk);
            int b = pop(stk);
        push(stk, a*b);
        }else if (strcmp(buffer, "+\n") == 0) {
            int a = pop(stk);
            int b = pop(stk);
            if (a == 0) {
                printf("You cannot divide a number by zero!\n");
                push(stk, b);
            }else {
                push(stk, b/a);
            }
        } else {
            int val = atoi(buffer);
            push(stk, val);
        }
    }
    printf("the result is: %d\n\n", pop(stk));
    printf("I love reversed polish notation, don't you?\n");
}