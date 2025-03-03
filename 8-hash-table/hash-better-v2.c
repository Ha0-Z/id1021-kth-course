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
    area **areas;
    int n;
} codes;

// Total numbers of adress in sweden:10000      Total possible adress:100000 
const int AREAS =40000;    // Numbers of pysical areas on array
const int BUFFER = 200;
int MOD = 3079;    // Numbers of area after modulo

int hash (int zip , int mod) {
    return (zip % mod); 
} 

void hash_add(area **dad, area *a) {
    int adress = hash(a->zip, MOD);
    int original_address = adress;
    
    // Linear probing with wraparound to avoid going past the array
    while (dad[adress] != NULL) {
        adress = (adress + 1) % MOD;
        // Prevent infinite loop if table is full
        if (adress == original_address) {
            fprintf(stderr, "Hash table is full!\n");
            return;
        }
    }
    
    dad[adress] = a;
}

codes *read_postcodes(char *file) {
    codes *postnr = (codes*)malloc(sizeof(codes));
    // Allocate array of pointers to areas and initialize to NULL
    area **areas = (area**)calloc(AREAS, sizeof(area*));
    
    FILE *fptr = fopen(file, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Failed to open file %s\n", file);
        free(areas);
        free(postnr);
        return NULL;
    }
    
    int k = 0;
    char *lineptr = malloc(sizeof(char) * BUFFER);
    size_t n = BUFFER;

    while ((k < AREAS) && (getline(&lineptr, &n, fptr) > 0)) {
        // Allocate a permanent area structure
        area *a = (area*)malloc(sizeof(area));
        
        // Make a copy of the line for tokenization
        char *copy = strdup(lineptr);
        
        // Parse fields
        char *zip_str = strtok(copy, ",");
        a->zip = atoi(zip_str)*100 + atoi(zip_str+3);
        
        char *name = strtok(NULL, ",");
        a->name = strdup(name);  // Make a permanent copy of the name
        
        a->pop = atoi(strtok(NULL, ","));
        
        // Add to hash table
        hash_add(areas, a);
        
        free(copy);  // Free the temporary copy
        k++;
    }
    
    free(lineptr);
    fclose(fptr);
    postnr->areas = areas;
    postnr->n = k;

    return postnr;
}

area *lookup(codes *body, int zip) {
    area **dad = body->areas;
    int adress = hash(zip, MOD);
    int original_address = adress;
    
    printf("Start to lookup: \n");

    // Check for NULL before dereferencing
    while (dad[adress] != NULL) {
        printf("Current: %d \n", dad[adress]->zip);
        
        if (dad[adress]->zip == zip) {
            return dad[adress];
        }
        
        adress = (adress + 1) % MOD;  // Wraparound to prevent array overflow
        
        // If we've come back to where we started, the item isn't in the table
        if (adress == original_address) {
            break;
        }
    }
    
    return NULL;  // Not found
}

int main() {
    srand(time(NULL));
    codes *table = read_postcodes("postnummer.csv");
    if (table == NULL) {  // FIXED: Check if table was successfully created
        return 1;  // Exit with an error code
    }
    area *test = lookup(table, 11759);
    lookup(table, 98333);
    lookup(table, 97598);

    if (test != NULL) {  // FIXED: Check if area was found
        printf("%s\n", test->name);  // Print the area name
    } else {
        printf("Area not found.\n");  // If not found, print an error message
    }

    return 0;
}
