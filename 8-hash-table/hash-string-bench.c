#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>
#include <string.h>

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

// Function to generate sorted integer array
int *sorted_array(int size) {
    int *numbers = (int*)malloc(size * sizeof(int));
    int current_value = 0;
    for (int i = 0; i < size; i++) {
        current_value += rand() % 10 + 1;
        numbers[i] = current_value;
    }
    return numbers;
}

// Function to generate unsorted integer array
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


typedef struct area {
    char *name;
    char *zip;
    int pop;
} area;

typedef struct codes {
    area *areas;
    int n;
} codes;

const int AREAS = 10000;
const int BUFFER = 200;

codes *read_postcodes(char *file) {
    codes *postnr = (codes*)malloc(sizeof(codes));
    area *areas = (area*)malloc(sizeof(area)*AREAS);
    FILE *fptr = fopen(file, "r");
    int k = 0;
    char *lineptr = malloc(sizeof(char)*BUFFER);
    size_t n = BUFFER;
    while((k < AREAS) && (getline(&lineptr, &n, fptr) > 0)) {
        char *copy = (char*)malloc(sizeof(char)*n);
        strcpy(copy, lineptr);
        area a;
        a.zip = strtok(copy, ",");
        a.name = strtok(NULL, ",");
        a.pop = atoi(strtok(NULL, ","));
        areas[k++] = a;
    }
    fclose(fptr);
    postnr->areas = areas;
    postnr->n = k;
    return postnr;
}

area *linear_search(codes *body, char *goal) {
    area *list = body->areas;
    for (int i = 0; i < body->n; i++)
    {
        area *test = &list[i];
        if(strcmp(test->zip,goal) == 0) {
            return test;
        }
        //printf("%s \n", test->zip);
    }
    printf("LINEAR SEARCH DID NOT FOUND, ZIP: %s \n", goal);
    return NULL;
}

area *binary_search(codes *body, char *goal) {
    int head = 0;
    int tail = body->n -1;
    int mid = head + (tail - head) / 2;
    area *mid_area = &body->areas[mid];
    int buffer = strcmp(mid_area->zip,goal);
    while (head <= tail)
    {   
        if (buffer == 0)
        {
            // Found the item
            return mid_area;
        }
        if (buffer > 0) {
            // Goal is larger than current
            tail = mid -1;
        } else {
            // Goal is smaller than current
            head = mid + 1;
        }

        mid = head + (tail - head) / 2;
        mid_area = &body->areas[mid];
        buffer = strcmp(mid_area->zip,goal);
    }
    printf("BINARY SEARCH DID NOT FOUND, AREA: %s \n", goal);
    return NULL;
}

void free_codes(codes *postcodes) {
    if (postcodes == NULL) return;
    if (postcodes->areas != NULL) {
        free(postcodes->areas);
    }
    free(postcodes);
}

void bench_search(int min_searches, int max_searches, int num_iterations, char *filename) {
    printf("Running benchmark on searches from %d to %d\n", min_searches, max_searches);
    printf("SearchType   NumSearches   TimeMin(ms)   TimeMax(ms)   Average(ms)  Average/NumSearches(ns)\n");

    int search_counts[] = {min_searches, 2 * min_searches, 4 * min_searches, 8 * min_searches, max_searches}; // Example search counts

    for (int s = 0; s < sizeof(search_counts) / sizeof(search_counts[0]); ++s) {
        int num_searches = search_counts[s];

        long linear_min_time = LONG_MAX;
        long linear_max_time = 0;
        long linear_total_time = 0;

        long binary_min_time = LONG_MAX;
        long binary_max_time = 0;
        long binary_total_time = 0;


        for (int iteration = 0; iteration < num_iterations; iteration++) {
            codes *table = read_postcodes(filename);
            if (table == NULL) {
                fprintf(stderr, "Failed to read postcode data, skipping iteration.\n");
                continue;
            }

            if (table->n == 0) {
                fprintf(stderr, "No data loaded from postcode file, skipping iteration.\n");
                free_codes(table);
                continue;
            }


            // Generate random search targets (zip codes from the table)
            char **search_targets = (char**)malloc(num_searches * sizeof(char*));
            if (search_targets == NULL) {
                perror("Failed to allocate search_targets");
                free_codes(table);
                continue;
            }
            for (int i = 0; i < num_searches; i++) {
                int random_index = rand() % table->n;
                search_targets[i] = strdup(table->areas[random_index].zip); //important to use strdup to copy
                if (search_targets[i] == NULL) {
                    perror("Failed to allocate search_targets[i]");
                    for(int j = 0; j < i; ++j) free(search_targets[j]); // Clean up already allocated targets
                    free(search_targets);
                    free_codes(table);
                    goto next_iteration; // Skip to next iteration if allocation fails
                }
            }


            // Benchmark linear search
            struct timespec start_time_linear, end_time_linear;
            clock_gettime(CLOCK_MONOTONIC, &start_time_linear);
            for (int search_count = 0; search_count < num_searches; search_count++) {
                linear_search(table, search_targets[search_count]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end_time_linear);
            long wall_linear = micro_seconds(&start_time_linear, &end_time_linear);

            linear_min_time = (wall_linear < linear_min_time) ? wall_linear : linear_min_time;
            linear_max_time = (wall_linear > linear_max_time) ? wall_linear : linear_max_time;
            linear_total_time += wall_linear;


            // Benchmark binary search
            struct timespec start_time_binary, end_time_binary;
            clock_gettime(CLOCK_MONOTONIC, &start_time_binary);
            for (int search_count = 0; search_count < num_searches; search_count++) {
                binary_search(table, search_targets[search_count]);
            }
            clock_gettime(CLOCK_MONOTONIC, &end_time_binary);
            long wall_binary = micro_seconds(&start_time_binary, &end_time_binary);

            binary_min_time = (wall_binary < binary_min_time) ? wall_binary : binary_min_time;
            binary_max_time = (wall_binary > binary_max_time) ? wall_binary : binary_max_time;
            binary_total_time += wall_binary;


            // Clean up search targets
            for (int i = 0; i < num_searches; i++) {
                free(search_targets[i]);
            }
            free(search_targets);
            free_codes(table);

            next_iteration:; // Label to jump to on allocation failure
        }


        printf("LinearSearch   %d   %0.2f   %0.2f   %0.2f   %0.2f\n",
               num_searches,
               (double)linear_min_time / 1.0,
               (double)linear_max_time / 1.0,
               (double)linear_total_time / num_iterations / 1.0,
               (double)linear_total_time / num_iterations / num_searches * 1000)
               ;

        printf("BinarySearch   %d   %0.2f   %0.2f   %0.2f   %0.2f\n",
               num_searches,
               (double)binary_min_time / 1.0,
               (double)binary_max_time / 1.0,
               (double)binary_total_time / num_iterations / 1.0,
               (double)binary_total_time / num_iterations / num_searches * 1000)
               ;
    }
}


int main() {
    srand(time(NULL));
    // Example CSV file name, replace with your actual file path
    char *filename = "postnummer.csv";



    bench_search(1000, 10000, 10, filename); // Example with searches from 100 to 1000, 10 iterations

    return 0;
}
