#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}

int *sorted(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
    int current_value = 0;
    for (int i = 0; i < size; i++) {
        current_value += rand() % 10 + 1;
        numbers[i] = current_value;
    }
    return numbers;
}

int *unsorted_array(int size) {
    int *numbers = sorted(size);
    for (int i = 0; i < size; i++) {
        int random_index = rand() % size;
        int temp = numbers[i];
        numbers[i] = numbers[random_index];
        numbers[random_index] = temp;
    }
    return numbers;
}


void check_sorted(int *array, int array_length) {
    for (int i = 1; i < array_length; i++) {
        if (array[i] < array[i - 1]) {
            printf("not sorted\n");
            return;  
        }
    }
    return;
}

void swap(int *array, int item_index, int item_index2) {
    int temp = array[item_index];
    array[item_index] = array[item_index2];
    array[item_index2] = temp;
}

int selection_sort(int *array, int array_length) {
    for (int i = 0; i < array_length - 1; i++) {
        int candidate = i;  
        for (int j = i + 1; j < array_length; j++) {
            if (array[j] < array[candidate]) {
                candidate = j;  
            }
        }
        
        if (candidate != i) {
            swap(array,i,candidate);
        }
    }
    return 0;  
}



int insert_sort(int *array, int array_length) {
    for (int i = 1; i < array_length; i++) {
        for (int j = i; j > 0 && array[j] < array[j - 1]; j--) {
            swap(array, j, j - 1);  
        }
    }
    return 0; 
}



void merge(int *org, int *temp, int low, int mid, int high) {
// copy all items from low to high from org to temp
    for (int i = low; i <= high; i++) {
        temp[i] = org[i];
    }
    // let's do the merging
    int i = low; // the index in the first part
    int j = mid+1; // the index in the second part
    int index = low;

    // Repeating copy the lowest element between low part and high part to the original array
    while (i <= mid && j <= high)
    {
        if (temp[i] <= temp[j])
        {
            org[index] = temp[i];
            i++;
        } else {
            org[index] = temp[j];
            j++;
        }
        index++;
    }
    
    // Copy if there is ramain array elements
    while (i <= mid) {
        org[index] = temp[i];
        i++;
        index++;
    }

    // Copy if there is ramain array elements
    while (j <= high) {
        org[index] = temp[j];
        j++;
        index++;
    }
}

void merge_sort_recursive(int *org, int *temp, int low, int high) {
    if (low < high) {
        int mid = (low + high)/2;
        // sort the items from low to mid
        merge_sort_recursive(org, temp, low, mid);
        // sort the items from mid+1 to high
        merge_sort_recursive(org, temp, mid+1, high);
        // merge the two sections using the additional array
        merge(org, temp, low, mid, high);
    }
}

void merge_sort(int *org, int n) {
    if (n == 0){
        return;
    }
    int *temp = (int*)malloc(n * sizeof(int));
    merge_sort_recursive(org, temp, 0, n-1);
    free(temp);
}


void quick_sort_recursive(int *array, int low, int high) {
    if (low < high)
    {
        int pivot = array[high];  // Choose the last element as the pivot
        int i = low - 1;       // Index of the smaller element

        for (int j = low; j < high; j++) {
            // If the current element is smaller than or equal to the pivot
            if (array[j] <= pivot) {
                i++;  // Increment the index of the smaller element
                swap(array,i, j);
            }
        }
        // Place the pivot in the correct position
        swap(array, i+1, high);
        pivot = i + 1;

        // Recursively sort the left and right subarrays
        quick_sort_recursive(array, low, pivot - 1);
        quick_sort_recursive(array, pivot + 1, high);

    }
    
}

void quick_sort(int *array, int length) {
    quick_sort_recursive(array, 0, length-1);
}



void bench(int num_searches, int min_array_size, int max_array_size, int num_iterations) {
    printf("Running %d searches on arrays from size %d to %d\n", num_searches, min_array_size, max_array_size);
    printf("Loops   Array size   TimeMin   TimeMax   Average\n");

    for(int current_size = min_array_size; current_size < max_array_size; current_size *= 2) {
        long min_time = LONG_MAX;
        long max_time = 0;
        long total_time = 0;

        for (int iteration = 0; iteration < num_iterations; iteration++) {

            // DATASET
            int *test_array = unsorted_array(current_size);


            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            // CODE TO BENCHMARK
            quick_sort(test_array,current_size);

            clock_gettime(CLOCK_MONOTONIC, &end_time);
            long wall = nano_seconds(&start_time, &end_time);
            
            min_time = (wall < min_time) ? wall : min_time;
            max_time = (wall > max_time) ? wall : max_time;
            total_time += wall;
            
            free(test_array);
        }

        printf("%d   %d   %0.1fns   %0.1fns   %0.1fns \n", 
            num_searches, current_size, 
            (double)min_time / num_searches, 
            (double)max_time / num_searches, 
            (double)total_time / num_iterations / num_searches);
    }
} 

int main() {
    bench(1, 100, 20000, 50);
    return 0;
}