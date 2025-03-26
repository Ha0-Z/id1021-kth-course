#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER 1024
#define MOD 307 // Hash table size (prime number)
#define MAX_CITIES 202 // Max number of cities for path tracking
#define INT_MAX 10000000;


long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}


// Helper function to calculate time difference in milliseconds
long time_diff_ms(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000 + 
           (end.tv_usec - start.tv_usec) / 1000;
}

// Structure to represent a connection to another city
typedef struct connection {
    struct city *destination;
    int distance;
    struct connection *next;
} conn;

// Structure to represent a city
typedef struct city {
    char *name;
    int id; 
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

typedef struct heap_node {
    city *city;
    city *previous_city;
    int distance;
    struct heap_node next;
} heap_node;

typedef struct heap {
    heap_node *start;
    heap_node *current;
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
map* graph(char *file) {
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

// Check if a city is in the current path (for loop detection)
int is_in_path(city **path, int path_length, city *check) {
    for (int i = 0; i < path_length; i++) {
        if (path[i] == check) {
            return 1; // City is in the path
        }
    }
    return 0; // City is not in the path
}

// Check if a city is in the current path (for loop detection)
int loop(city **path, int k, city *dst) {
    for (int i = 0; i < k; i++) {
        if (path[i] == dst) {
            return 1; // City is in the path (loop detected)
        }
    }
    return 0; // No loop detected
}

heap_node* create_queue(city *item) {
    heap_node *new_node = (heap_node*)malloc(sizeof(heap_node));
    new_node->city = item;
    new_node->distance = 0;
    new_node->next = NULL;
    return new_node;
}

void enque(heap *queue, city *item, city *previous, int distance) {
    if (queue == NULL) {
        heap *new_heap = (heap*)malloc(sizeof(heap));
        new_heap->start = create_queue(item);
        new_heap->current = new_heap->start;
        new_heap->current->distance = distance;
        new_heap->current->previous_city = NULL;
        return;
    }

    heap_node *new_node = (heap_node*)malloc(sizeof(heap_node));
    new_node->city = item;
    new_node->distance = 0;

    heap_node *buffer = queue->current->next;

    while(buffer->next != NULL) {
        if (buffer->next->distance > distance)
        {
            heap *next = buffer->next;
            buffer->next = new_node;
            new_node->next = next;
            return;
        }
        
    }
    buffer->next = new_node;
    new_node->next = NULL;
}

void print_path(heap_node *test) {
    
    while (test != NULL)
    {
        printf("%s, " , test->city);
        test = test->previous_city;
    }
    
}

int dijkstra (city *from, city *to) {
    if (from == to)
    {
        return 0;
    }
    int done[MAX_CITIES];
    heap *queue = (heap*)malloc(sizeof(heap));
    enque(queue,from,NULL,0);
    int goal = INT_MAX;
    heap_node *goal_city = NULL;
    conn *city_list;
    int current_distance = 0;

    while(queue->current->distance >goal) {
        city_list = queue->current->city->connections;
        current_distance = queue->current->distance;
        while(city_list != NULL) {
            enque(queue,city_list->destination,queue->current->city,current_distance+city_list->distance);
            city_list = city_list->next;
            done[queue->current->city->id] = 1;
        }
        if (queue->current == to)
        {
            goal = queue->current->distance;
            goal_city = queue->current->distance;
        }
        queue->current = queue->current->next;
    
    }
    print_path(goal_city);
    return goal;
}

// Wrapper function for shortest_path
int search(city *from, city *to) {
    if (from == to) return 0;
    
    city *path[MAX_CITIES];
    return shortest_path(from, to, path, 0);
}


// Free all allocated memory
void free_graph(map *trains) {
    if (!trains) return;
    
    // Free each bucket in the hash table
    for (int i = 0; i < MOD; i++) {
        city_node *node = trains->buckets[i];
        
        while (node != NULL) {
            city_node *temp_node = node;
            city *city_ptr = node->city_ptr;
            node = node->next;
            
            // Free all connections of this city
            conn *connection = city_ptr->connections;
            while (connection != NULL) {
                conn *temp_conn = connection;
                connection = connection->next;
                free(temp_conn);
            }
            
            // Free the city and its name
            free(city_ptr->name);
            free(city_ptr);
            
            // Free the node
            free(temp_node);
        }
    }
    
    // Free the buckets array and the map
    free(trains->buckets);
    free(trains);
}


int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <trains.csv> <from_city> <to_city>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    map *trains = graph(argv[1]);
    if (!trains) {
        fprintf(stderr, "Failed to load the graph.\n");
        return EXIT_FAILURE;
    }
    
    city *from = lookup(trains, argv[2]);
    city *to = lookup(trains, argv[3]);
    
    if (!from || !to) {
        fprintf(stderr, "City not found.\n");
        free_graph(trains);
        return EXIT_FAILURE;
    }
    
    printf("Finding shortest path from %s to %s...\n", from->name, to->name);
    
    // Test with improved shortest path algorithm (with loop detection and dynamic limit)
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int shortest_distance = search(from, to);
    
    clock_gettime(CLOCK_MONOTONIC, &end);    
    long time_ns = nano_seconds(&start, &end);
    
    if (shortest_distance >= 0) {
        printf("Shortest path: %d minutes (found in %.0f ns)\n", 
               shortest_distance, (double)time_ns);
    } else {
        printf("No path found!\n");
    }
    
    // Clean up
    free_graph(trains);
    
    return EXIT_SUCCESS;
}