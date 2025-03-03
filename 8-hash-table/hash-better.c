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
const int AREAS =20000;    // Numbers of pysical areas on array
const int BUFFER = 200;
int MOD = 3079;    // Numbers of area after modulo

int hash (int zip , int mod) {
    return (zip % mod);
} 

void hash_add(area **dad, area *a) {
    int adress = hash(a->zip,MOD);
    while (dad[adress] != NULL)
    {
        adress++;
    }
    
    dad[adress] = a;

}

codes *read_postcodes(char *file) {
    codes *postnr = (codes*)malloc(sizeof(codes));
    area **areas = (area**)malloc(sizeof(area) * AREAS);
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

        hash_add(areas, &a);

        // printf("City: %s, Zip: %d, hash: %d\n", a.name, a.zip, hash(a.zip,MOD));
    }
    

    fclose(fptr);
    postnr->areas = areas;
    postnr->n = k;

    return postnr;
}

area *lookup(codes *body, int zip){
    area ** dad = body->areas;
    int adress = hash(zip,MOD);
    dad[adress];

    
    printf("Start to lookup: \n");

    while (dad[adress]->zip != zip )
    {
        printf("Current: %d \n", dad[adress]->zip);
        if (dad[adress] == NULL)
        {
            return NULL;
        }
        adress++;
    }
    return dad[adress];
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
