#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>

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
// int main()
// {
//     stack *stk = new_stack(4);
//     push(stk, 32);
//     push(stk, 33);
//     push(stk, 34);
//     printf("pop : %d\n", pop(stk));
//     printf("pop : %d\n", pop(stk));
//     printf("pop : %d\n", pop(stk));
// }

 
//  Does the pointer point to the location above the top of the stack or does it point to the top of the stack?
// The pointer points at the top element of the stack. 

//  What is the value of the pointer when the stack is empty?
// If the stack is empty, then the pointer point towards -1, which do not exist.

//  What should you do when a program tries to push a value on a full stack (stack overflow)?
// If the program tries to push if the stack is full. Nothing happens and "Stack is full!" will be printed.

//  What should happen when someone pops an item from an empty stack?
// Because the program expect a value be returned, there fore a message "Cannot pop a empty stack!" and -1 will be returned.


long nano_seconds(struct timespec *t_start, struct timespec *t_stop)
{
    return (t_stop->tv_nsec - t_start->tv_nsec) +
           (t_stop->tv_sec - t_start->tv_sec) * 1000000000;
}


int main() {

    // Create a for loop which have index of n add doubles after each iteration
    for(int n = 1000; n <= 128000; n = n * 2) {
        // Create a array with malloc with n elements;
        int *list = (int *)malloc(n * sizeof(int));
        for(int i = 0; i < n; i++) {
            list[i] = rand();
        }
        // Create a stack with new_stack(0);
        printf("initial stack length is: %d \n", n);
        stack *myStack = new_stack(n);

        long timeMin = LONG_MAX;

        for (int i = 0; i < 50; i++) {

            struct timespec t_start, t_stop;
            clock_gettime(CLOCK_MONOTONIC, &t_start);
                for(int i = 0; i < n; i++) {
                    push(myStack, list[i]);
                }
                // Pop x time from the stack
                for(int i = 0; i < n; i++) {
                    pop(myStack);
                }

            clock_gettime(CLOCK_MONOTONIC, &t_stop);
            long wall = nano_seconds(&t_start, &t_stop);
            if (wall < timeMin) {
                timeMin = wall;
            }
        }

        printf("%d %0.2f %0.2f ns\n", n, (double)timeMin, (double)timeMin / n);
        free(list);
        free(myStack);
    }

}

