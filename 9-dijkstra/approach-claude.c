#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER 1024
#define MOD 307 // Hash table size (prime number)
#define MAX_CITIES 202 // Max number of cities for path tracking
#define INT_MAX 2147483647 // Proper integer max

// Helper function to calculate nano seconds
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

// Structure to represent a path entry (as described in the assignment)
typedef struct path {
    city *city;
    city *prev;
    int dist;
} path;

// Structure to represent a node in our priority queue
typedef struct pq_node {
    path data;
    struct pq_node *next;
} pq_node;

// Structure to represent a priority queue
typedef struct priority_queue {
    pq_node *head;
    int size;
} priority_queue;

// Initialize a priority queue
priority_queue* pq_init() {
    priority_queue *pq = (priority_queue*)malloc(sizeof(priority_queue));
    if (!pq) {
        perror("Failed to allocate priority queue");
        exit(EXIT_FAILURE);
    }
    pq->head = NULL;
    pq->size = 0;
    return pq;
}

// Insert a path into the priority queue (ordered by distance)
void pq_insert(priority_queue *pq, city *city_ptr, city *prev, int dist) {
    pq_node *new_node = (pq_node*)malloc(sizeof(pq_node));
    if (!new_node) {
        perror("Failed to allocate priority queue node");
        exit(EXIT_FAILURE);
    }
    
    new_node->data.city = city_ptr;
    new_node->data.prev = prev;
    new_node->data.dist = dist;
    
    // Case: Empty queue or new node has lower distance than head
    if (pq->head == NULL || dist < pq->head->data.dist) {
        new_node->next = pq->head;
        pq->head = new_node;
    } else {
        // Find the correct position to insert
        pq_node *current = pq->head;
        while (current->next != NULL && current->next->data.dist <= dist) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
    
    pq->size++;
}

// Remove and return the path with the smallest distance
path pq_extract_min(priority_queue *pq) {
    if (pq->head == NULL) {
        fprintf(stderr, "Error: Attempting to extract from empty priority queue\n");
        exit(EXIT_FAILURE);
    }
    
    pq_node *min_node = pq->head;
    path result = min_node->data;
    
    pq->head = min_node->next;
    free(min_node);
    pq->size--;
    
    return result;
}

// Check if the priority queue is empty
int pq_is_empty(priority_queue *pq) {
    return pq->head == NULL;
}

// Free all memory used by the priority queue
void pq_free(priority_queue *pq) {
    pq_node *current = pq->head;
    while (current != NULL) {
        pq_node *temp = current;
        current = current->next;
        free(temp);
    }
    free(pq);
}

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
    new_city->id = trains->city_count; // Assign a unique ID to the city
    
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

// Print the path from source to destination using the done array
void print_path(path *done, city *src, city *dst) {
    if (src == dst) {
        printf("%s", src->name);
        return;
    }
    
    // Create an array to store the cities in the path
    city *path_array[MAX_CITIES];
    int path_length = 0;
    
    // Start from the destination and work backwards
    city *current = dst;
    while (current != src) {
        path_array[path_length++] = current;
        
        // Get the previous city from the done array
        int id = current->id;
        current = done[id].prev;
        
        if (current == NULL) {
            printf("No path exists!\n");
            return;
        }
    }
    path_array[path_length++] = src;
    
    // Print the path in correct order (source to destination)
    printf("Path: ");
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", path_array[i]->name);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

// Implementation of Dijkstra's algorithm
int dijkstra(city *src, city *dst, map *trains) {
    // Handle the case where source and destination are the same
    if (src == dst) {
        printf("Source and destination are the same city: %s\n", src->name);
        return 0;
    }
    
    // Create the "done" array to track processed cities
    path *done = (path*)malloc(trains->city_count * sizeof(path));
    if (!done) {
        perror("Failed to allocate done array");
        exit(EXIT_FAILURE);
    }
    
    // Initialize all entries in the done array
    for (int i = 0; i < trains->city_count; i++) {
        done[i].city = NULL;
        done[i].prev = NULL;
        done[i].dist = INT_MAX;
    }
    
    // Create priority queue
    priority_queue *pq = pq_init();
    
    // Add source city to the priority queue with distance 0
    pq_insert(pq, src, NULL, 0);
    
    // Main Dijkstra algorithm loop
    while (!pq_is_empty(pq)) {
        // Extract the path with minimum distance
        path current = pq_extract_min(pq);
        city *current_city = current.city;
        int current_dist = current.dist;
        
        // If we've reached the destination, we're done
        if (current_city == dst) {
            // Save the result in the done array for path reconstruction
            done[dst->id] = current;
            
            // Print the path
            printf("Shortest path found:\n");
            print_path(done, src, dst);
            
            // Free resources
            pq_free(pq);
            int result = current_dist;
            free(done);
            return result;
        }
        
        // Skip if we've already processed this city with a shorter path
        if (done[current_city->id].city != NULL && done[current_city->id].dist <= current_dist) {
            continue;
        }
        
        // Mark this city as processed with the current shortest path
        done[current_city->id] = current;
        
        // Process all connections from the current city
        conn *connection = current_city->connections;
        while (connection != NULL) {
            city *neighbor = connection->destination;
            int new_dist = current_dist + connection->distance;
            
            // If we found a shorter path to this neighbor, add it to the queue
            if (done[neighbor->id].city == NULL || new_dist < done[neighbor->id].dist) {
                pq_insert(pq, neighbor, current_city, new_dist);
            }
            
            connection = connection->next;
        }
    }
    
    // If we get here, no path was found
    printf("No path found from %s to %s\n", src->name, dst->name);
    pq_free(pq);
    free(done);
    return -1;
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
    
    // Measure execution time
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Run Dijkstra's algorithm
    int shortest_distance = dijkstra(from, to, trains);
    
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