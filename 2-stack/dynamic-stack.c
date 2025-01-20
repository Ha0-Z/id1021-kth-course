#include <stdlib.h>
#include <stdio.h>

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
        printf("Size increase to: %d\n", newSize);
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
            
            printf("Size decreases to: %d\n", newSize);
        }   
        stk->top = stk->top - 1;
        return stk->array[(stk->top + 1)];
    }
    

}


int main() {
    stack *stk = new_stack(4);
    int n = 10;
    for(int i = 0; i < n; i++) {
        push(stk, i+30);
    }
    for(int i = 0; i < stk->top; i++) {
        printf("stack[%d] : %d\n", i, stk->array[i]);
    }
    int val = pop(stk);
    while(val != -2147483647) { // assuming -2147483647 is returned when the stack is empty
        printf("pop : %d\n", val);
        val = pop(stk);
    }
}

