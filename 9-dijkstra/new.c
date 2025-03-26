#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER 1024
#define MOD 389 // Hash table size (prime number)
#define MAX_CITIES 202 // Max number of cities for path tracking


typedef struct path {
    city *city;
    city *previous;
    int distance;
} path;


typedef struct connection {
    struct city *destination;
    int distance;
    struct connection *next;
} conn;

// Structure to represent a city
typedef struct city {
    char *name;
    conn *connections;
} city;

// Structure to represent a node in the hash table bucket
typedef struct city_node {
    city *city_ptr;
    struct city_node *next;
} city_node;

// Structure to represent the map (using a hash table of city nodes)
typedef struct map {
    city_node **buckets;
    int city_count;
} map;

typedef struct heap {
    path city;
    int distance;
    struct heap left;
    struct heap right;
} heap;

// Hash function for city names
int hash(char *name, int mod) {
    int h = 0;
    int i = 0;
    unsigned char c = 0;
    
    while ((c = name[i]) != 0) {
        h = (h * 31 + c) % mod;
        i++;
    }
    return h;
}

// Add a bidirectional connection between two cities
void connect(city *src, city *dst, int distance) {
    // Add connection from src to dst
    conn *new_conn_src = (conn*)malloc(sizeof(conn));
    if (!new_conn_src) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    new_conn_src->destination = dst;
    new_conn_src->distance = distance;
    new_conn_src->next = src->connections;
    src->connections = new_conn_src;
    
    // Add connection from dst to src (bidirectional)
    conn *new_conn_dst = (conn*)malloc(sizeof(conn));
    if (!new_conn_dst) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    new_conn_dst->destination = src;
    new_conn_dst->distance = distance;
    new_conn_dst->next = dst->connections;
    dst->connections = new_conn_dst;
}

// Look up a city in the hash table, create if not found
city* lookup(map *trains, char *name) {
    int index = hash(name, MOD);
    city_node *current = trains->buckets[index];
    
    // Search for the city in the bucket
    while (current != NULL) {
        if (strcmp(current->city_ptr->name, name) == 0) {
            return current->city_ptr; // City found
        }
        current = current->next;
    }
    
    // City not found, create a new one
    city *new_city = (city*)malloc(sizeof(city));
    if (!new_city) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    new_city->name = strdup(name);
    if (!new_city->name) {
        perror("strdup failed");
        free(new_city);
        exit(EXIT_FAILURE);
    }
    
    new_city->connections = NULL;
    
    // Add the new city to the hash table
    city_node *new_node = (city_node*)malloc(sizeof(city_node));
    if (!new_node) {
        perror("malloc failed");
        free(new_city->name);
        free(new_city);
        exit(EXIT_FAILURE);
    }
    
    new_node->city_ptr = new_city;
    new_node->next = trains->buckets[index];
    trains->buckets[index] = new_node;
    
    trains->city_count++;
    return new_city;
}

// Build the graph from the CSV file
map* graph() {
    FILE *fptr = fopen("europe.csv", "r");
    // Allocate and initialize the map
    map *trains = (map*)malloc(sizeof(map));
    if (!trains) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    trains->buckets = (city_node**)calloc(MOD, sizeof(city_node*));
    if (!trains->buckets) {
        perror("calloc failed");
        free(trains);
        exit(EXIT_FAILURE);
    }
    
    trains->city_count = 0;
    
    // Open the file
    FILE *fptr = fopen(file, "r");
    if (!fptr) {
        perror("Error opening file");
        free(trains->buckets);
        free(trains);
        return NULL;
    }
    
    char *lineptr = NULL;
    size_t n = 0;
    size_t read;
    
    // Process each line in the file
    while ((read = getline(&lineptr, &n, fptr)) != -1) {
        // Remove trailing newline
        if (read > 0 && lineptr[read - 1] == '\n') {
            lineptr[read - 1] = '\0';
        }
        
        char *copy = strdup(lineptr);
        if (!copy) {
            perror("strdup failed");
            continue;
        }
        
        // Parse the CSV line
        char *from_name = strtok(copy, ",");
        char *to_name = strtok(NULL, ",");
        char *dist_str = strtok(NULL, ",");
        
        if (from_name && to_name && dist_str) {
            city *src = lookup(trains, from_name);
            city *dst = lookup(trains, to_name);
            int dist = atoi(dist_str);
            
            connect(src, dst, dist);
        }
        
        free(copy);
    }
    
    free(lineptr);
    fclose(fptr);
    return trains;
}

