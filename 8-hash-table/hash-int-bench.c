#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>

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

typedef struct area {
    char *name;
    int zip;  // Changed to an int instead of a pointer
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
    area *areas = (area*)malloc(sizeof(area) * AREAS);
    FILE *fptr = fopen(file, "r");
    int k = 0;
    char *lineptr = malloc(sizeof(char) * BUFFER);
    size_t n = BUFFER;
    
    while ((k < AREAS) && (getline(&lineptr, &n, fptr) > 0)) {
        char *copy = (char*)malloc(sizeof(char) * n);
        strcpy(copy, lineptr);
        area a;

        char *zip = strtok(copy, ",");
        a.zip = atoi(zip)*100 + atoi(zip+3);
        a.name = strtok(NULL, ",");
        a.pop = atoi(strtok(NULL, ","));
        areas[k++] = a;
    }

    fclose(fptr);
    postnr->areas = areas;
    postnr->n = k;
    return postnr;
}

area *linear_search(codes *body, int goal) {
    area *list = body->areas;
    for (int i = 0; i < body->n; i++) {
        area *test = &list[i];
        if (test->zip == goal) {  // Changed from strcmp to integer comparison
            return test;
        }
    }
    printf("LINEAR SEARCH DID NOT FOUND, ZIP: %d \n", goal);  // Changed format to %d
    return NULL;
}

area *binary_search(codes *body, int goal) {
    int head = 0;
    int tail = body->n - 1;
    while (head <= tail) {
        int mid = head + (tail - head) / 2;
        area *mid_area = &body->areas[mid];
        if (mid_area->zip == goal) {  // Changed from strcmp to integer comparison
            return mid_area;
        }
        if (mid_area->zip > goal) {
            tail = mid - 1;
        } else {
            head = mid + 1;
        }
    }
    printf("BINARY SEARCH DID NOT FIND, ZIP: %d \n", goal);  // Changed format to %d
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
            int *search_targets = (int*)malloc(num_searches * sizeof(int)); // Changed to int*
            if (search_targets == NULL) {
                perror("Failed to allocate search_targets");
                free_codes(table);
                continue;
            }
            for (int i = 0; i < num_searches; i++) {
                int random_index = rand() % table->n;
                search_targets[i] = table->areas[random_index].zip; // Get int zip directly
            }


            // Benchmark linear search
            struct timespec start_time_linear, end_time_linear;
            clock_gettime(CLOCK_MONOTONIC, &start_time_linear);
            for (int search_count = 0; search_count < num_searches; search_count++) {
                linear_search(table, search_targets[search_count]); // Pass int target
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
                binary_search(table, search_targets[search_count]); // Pass int target
            }
            clock_gettime(CLOCK_MONOTONIC, &end_time_binary);
            long wall_binary = micro_seconds(&start_time_binary, &end_time_binary);

            binary_min_time = (wall_binary < binary_min_time) ? wall_binary : binary_min_time;
            binary_max_time = (wall_binary > binary_max_time) ? wall_binary : binary_max_time;
            binary_total_time += wall_binary;


            // Clean up search targets
            free(search_targets); // Free int array, no need to free individual strings
            free_codes(table);

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
