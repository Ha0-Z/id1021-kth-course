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

 return stk;
}
void push(stack *stk, int val)
{
    if (stk->top >= (stk->size - 1))    
    {
        // Do not push if stack is full.
        printf("Stack is full!\n");
    }
    else
    {
        // Push val to the top of the stack and increment top by one.
        stk->top = (stk->top + 1);
        stk->array[stk->top] = val;
    }
    
}
int pop(stack *stk)
{
    if (stk->top <= -1)
    {
        printf("Cannot pop a empty stack!\n");
        return -1;
    }
    else
    {
        stk->top = stk->top - 1;
        return stk->array[(stk->top + 1)];
    }
    

}
int main()
{
    stack *stk = new_stack(4);
    push(stk, 32);
    push(stk, 33);
    push(stk, 34);
    printf("pop : %d\n", pop(stk));
    printf("pop : %d\n", pop(stk));
    printf("pop : %d\n", pop(stk));
}

 
//  Does the pointer point to the location above the top of the stack or does it point to the top of the stack?
// The pointer points at the top element of the stack. 

//  What is the value of the pointer when the stack is empty?
// If the stack is empty, then the pointer point towards -1, which do not exist.

//  What should you do when a program tries to push a value on a full stack (stack overflow)?
// If the program tries to push if the stack is full. Nothing happens and "Stack is full!" will be printed.

//  What should happen when someone pops an item from an empty stack?
// Because the program expect a value be returned, there fore a message "Cannot pop a empty stack!" and -1 will be returned.
