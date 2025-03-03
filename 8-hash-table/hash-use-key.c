#include <stdio.h>
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
    area **zip_code_index;
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
    
    postnr->zip_code_index = map_area_to_array(postnr);
    return postnr;
}

area *lookup(codes body, int index){
    area *buffer = body.zip_code_index[index]; 
    return buffer;
}

area *map_area_to_array(codes *body) {
    area **container = (area**)malloc(sizeof(area*) * 100000);  // Pointer array
    for (size_t i = 0; i < body->n; i++) {
        area *item = &body->areas[i];  // Get the area at index 'i'
        container[item->zip] = item;  // Store the pointer to the area at the zip index
    }
    return container;
}



int main() {
    srand(time(NULL));
    codes *table = read_postcodes("postnummer.csv");



    return 0;
}
