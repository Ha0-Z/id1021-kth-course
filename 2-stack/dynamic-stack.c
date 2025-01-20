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
    if (stk->top == stk->size - 1) {
        int newSize = stk->size * 2;
        int *copy = (int *)malloc(newSize * sizeof(int));
        for (int i = 0; i < (stk->size); i++) {
            copy[i] = stk->array[i];
        }
        free(stk->array);

        stk->size = newSize;
        stk->array = copy;
        // printf("Size increase to: %d\n", newSize);
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
            
            // printf("Size decreases to: %d\n", newSize);
        }   
        stk->top = stk->top - 1;
        return stk->array[(stk->top + 1)];
    }
    

}


// int main() {
//     stack *stk = new_stack(4);
//     int n = 10;
//     for(int i = 0; i < n; i++) {
//         push(stk, i+30);
//     }
//     for(int i = 0; i < stk->top; i++) {
//         printf("stack[%d] : %d\n", i, stk->array[i]);
//     }
//     int val = pop(stk);
//     while(val != -2147483647) { // assuming -2147483647 is returned when the stack is empty
//         printf("pop : %d\n", val);
//         val = pop(stk);
//     }
// }

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
            list[i] = rand();;
        }
        // Create a stack with new_stack(0);
        stack *myStack = new_stack(1);

        long timeMin = LONG_MAX;

        for (int i = 0; i < 10; i++) {

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
    }
    free(list);
    free(myStack);

}

