#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MOD 101 // Hash table size (prime number)
#define MAX_CITIES 60 // Max number of cities for path tracking
#define DEBUG 1 // Enable debug logging

// Debug print function
void debug_print(const char* format, ...) {
    if (DEBUG) {
        va_list args;
        va_start(args, format);
        printf("[DEBUG] ");
        vprintf(format, args);
        va_end(args);
    }
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
    int id;  // Added for Dijkstra's algorithm
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

// Structure to represent a path for Dijkstra's algorithm
typedef struct path {
    city *city;
    city *prev;
    int dist;
} path;

// Structure for priority queue node
typedef struct pq_node {
    path *path;
    struct pq_node *next;
} pq_node;

// Structure for priority queue
typedef struct {
    pq_node *head;
} priority_queue;

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

// Print all cities in the map
void print_cities(map *trains) {
    debug_print("List of all cities in the map (%d total):\n", trains->city_count);
    for (int i = 0; i < MOD; i++) {
        city_node *current = trains->buckets[i];
        while (current != NULL) {
            debug_print("[ID: %d] %s with connections to: ", 
                      current->city_ptr->id, current->city_ptr->name);
            
            // Print connections
            conn *connection = current->city_ptr->connections;
            int conn_count = 0;
            while (connection != NULL) {
                if (conn_count > 0) debug_print(", ");
                debug_print("%s (%d min)", connection->destination->name, connection->distance);
                connection = connection->next;
                conn_count++;
            }
            
            if (conn_count == 0) {
                debug_print("NONE (isolated city!)");
            }
            debug_print("\n");
            
            current = current->next;
        }
    }
}

// Look up a city in the hash table, create if not found
city* lookup(map *trains, char *name) {
    int index = hash(name, MOD);
    city_node *current = trains->buckets[index];
    
    // Search for the city in the bucket
    while (current != NULL) {
        if (strcmp(current->city_ptr->name, name) == 0) {
            debug_print("Found existing city: %s (ID: %d)\n", name, current->city_ptr->id);
            return current->city_ptr; // City found
        }
        current = current->next;
    }
    
    // City not found, create a new one
    debug_print("Creating new city: %s (ID: %d)\n", name, trains->city_count);
    city *new_city = (city*)malloc(sizeof(city));
    if (!new_city) {
        perror("malloc failed for new city");
        exit(EXIT_FAILURE);
    }
    
    new_city->name = strdup(name);
    if (!new_city->name) {
        perror("strdup failed for city name");
        free(new_city);
        exit(EXIT_FAILURE);
    }
    
    new_city->connections = NULL;
    new_city->id = trains->city_count;  // Assign unique ID
    
    // Add the new city to the hash table
    city_node *new_node = (city_node*)malloc(sizeof(city_node));
    if (!new_node) {
        perror("malloc failed for new city node");
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

// Add a bidirectional connection between two cities
void connect(city *src, city *dst, int distance) {
    debug_print("Connecting %s to %s (%d minutes)\n", src->name, dst->name, distance);
    
    // Add connection from src to dst
    conn *new_conn_src = (conn*)malloc(sizeof(conn));
    if (!new_conn_src) {
        perror("malloc failed for new connection from src to dst");
        exit(EXIT_FAILURE);
    }
    
    new_conn_src->destination = dst;
    new_conn_src->distance = distance;
    new_conn_src->next = src->connections;
    src->connections = new_conn_src;
    
    // Add connection from dst to src (bidirectional)
    conn *new_conn_dst = (conn*)malloc(sizeof(conn));
    if (!new_conn_dst) {
        perror("malloc failed for new connection from dst to src");
        exit(EXIT_FAILURE);
    }
    
    new_conn_dst->destination = src;
    new_conn_dst->distance = distance;
    new_conn_dst->next = dst->connections;
    dst->connections = new_conn_dst;
}

// Build the graph from the CSV file
map* graph() {
    char *file = "europe.csv";
    debug_print("Starting to build graph from file: %s\n", file);
    
    // Allocate and initialize the map
    map *trains = (map*)malloc(sizeof(map));
    if (!trains) {
        perror("malloc failed for map structure");
        exit(EXIT_FAILURE);
    }
    
    trains->buckets = (city_node**)calloc(MOD, sizeof(city_node*));
    if (!trains->buckets) {
        perror("calloc failed for map buckets");
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
    
    debug_print("File opened successfully\n");
    
    char line[BUFFER];
    int line_count = 0;
    
    // Process each line in the file
    while (fgets(line, BUFFER, fptr) != NULL) {
        line_count++;
        
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        debug_print("Processing line %d: %s\n", line_count, line);
        
        char *copy = strdup(line);
        if (!copy) {
            perror("strdup failed for line copy");
            continue;
        }
        
        // Parse the CSV line
        char *from_name = strtok(copy, ",");
        char *to_name = strtok(NULL, ",");
        char *dist_str = strtok(NULL, ",");
        
        if (from_name && to_name && dist_str) {
            debug_print("Parsed: %s -> %s (%s minutes)\n", from_name, to_name, dist_str);
            city *src = lookup(trains, from_name);
            city *dst = lookup(trains, to_name);
            int dist = atoi(dist_str);
            
            connect(src, dst, dist);
        } else {
            debug_print("WARNING: Skipping malformed line: %s\n", line);
        }
        
        free(copy);
    }
    
    debug_print("Finished reading file. Processed %d lines.\n", line_count);
    fclose(fptr);
    
    // Print all cities and their connections for debugging
    print_cities(trains);
    
    return trains;
}

// Create a new priority queue
priority_queue* create_priority_queue() {
    debug_print("Creating priority queue\n");
    priority_queue *pq = (priority_queue*)malloc(sizeof(priority_queue));
    if (!pq) {
        perror("malloc failed for priority queue");
        exit(EXIT_FAILURE);
    }
    pq->head = NULL;
    return pq;
}

// Add a path to the priority queue (ordered by distance)
void enqueue(priority_queue *pq, path *p) {
    debug_print("Enqueuing path to %s (distance: %d)\n", p->city->name, p->dist);
    pq_node *new_node = (pq_node*)malloc(sizeof(pq_node));
    if (!new_node) {
        perror("malloc failed for priority queue node");
        exit(EXIT_FAILURE);
    }
    new_node->path = p;
    
    // If the queue is empty or the new path has a smaller distance than the head
    if (pq->head == NULL || p->dist < pq->head->path->dist) {
        new_node->next = pq->head;
        pq->head = new_node;
    } else {
        // Find the correct position based on distance
        pq_node *current = pq->head;
        while (current->next != NULL && current->next->path->dist <= p->dist) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// Remove and return the path with the smallest distance
path* dequeue(priority_queue *pq) {
    if (pq->head == NULL) {
        debug_print("Attempted to dequeue from empty priority queue\n");
        return NULL;
    }
    
    pq_node *temp = pq->head;
    path *p = temp->path;
    pq->head = pq->head->next;
    free(temp);
    
    debug_print("Dequeued path to %s (distance: %d)\n", p->city->name, p->dist);
    return p;
}

// Check if the priority queue is empty
int is_empty(priority_queue *pq) {
    return pq->head == NULL;
}

// Free the priority queue
void free_priority_queue(priority_queue *pq) {
    debug_print("Freeing priority queue\n");
    while (!is_empty(pq)) {
        path *p = dequeue(pq);
        free(p);
    }
    free(pq);
}

// Create a new path
path* create_path(city *city, struct city *prev, int dist) {
    debug_print("Creating path to %s (prev: %s, distance: %d)\n", 
              city->name, prev ? prev->name : "NULL", dist);
    path *p = (path*)malloc(sizeof(path));
    if (!p) {
        perror("malloc failed for path");
        exit(EXIT_FAILURE);
    }
    p->city = city;
    p->prev = prev;
    p->dist = dist;
    return p;
}

// Check if a city has any connections to other cities
int has_connections(city *c) {
    return c->connections != NULL;
}

// Print the contents of the done array
void print_done_array(path **done, int num_cities) {
    debug_print("Done array contents (%d cities):\n", num_cities);
    for (int i = 0; i < num_cities; i++) {
        if (done[i] != NULL) {
            debug_print("  [%d] City: %s, Prev: %s, Dist: %d\n", 
                      i, done[i]->city->name, 
                      done[i]->prev ? done[i]->prev->name : "NULL", 
                      done[i]->dist);
        } else {
            debug_print("  [%d] NULL (not visited)\n", i);
        }
    }
}

// Dijkstra's algorithm implementation
path** dijkstra(city *src, city *dest, int num_cities) {
    debug_print("\nStarting Dijkstra's algorithm from %s to %s (%d total cities)\n", 
              src->name, dest->name, num_cities);
    
    // Check if source and destination cities have connections
    if (!has_connections(src)) {
        debug_print("ERROR: Source city %s has no connections!\n", src->name);
    }
    if (!has_connections(dest)) {
        debug_print("ERROR: Destination city %s has no connections!\n", dest->name);
    }
    
    // Initialize done array to keep track of shortest paths
    path **done = (path**)calloc(num_cities, sizeof(path*));
    if (!done) {
        perror("calloc failed for done array");
        exit(EXIT_FAILURE);
    }
    
    debug_print("Allocated done array for %d cities\n", num_cities);
    
    // Initialize priority queue
    priority_queue *pq = create_priority_queue();
    
    // Add source city to priority queue
    path *src_path = create_path(src, NULL, 0);
    enqueue(pq, src_path);
    
    int iterations = 0;
    while (!is_empty(pq)) {
        iterations++;
        debug_print("\nIteration %d of Dijkstra's algorithm\n", iterations);
        
        // Get the path with the smallest distance
        path *current_path = dequeue(pq);
        city *current_city = current_path->city;
        
        debug_print("Processing city: %s (ID: %d, distance: %d)\n", 
                  current_city->name, current_city->id, current_path->dist);
        
        // If we've reached the destination, we're done
        if (current_city == dest) {
            debug_print("Reached destination %s! Path found with distance %d\n", 
                      dest->name, current_path->dist);
            
            // Save the path to the destination
            done[current_city->id] = current_path;
            
            // Print the final done array
            print_done_array(done, num_cities);
            
            // Free the priority queue
            free_priority_queue(pq);
            return done;
        }
        
        // If we've already found the shortest path to this city, skip it
        if (done[current_city->id] != NULL) {
            debug_print("Already found shortest path to %s, skipping\n", current_city->name);
            free(current_path);
            continue;
        }
        
        // Add the current path to the done array
                
        // Add the current path to the done array
        done[current_city->id] = current_path;
        
        // Explore neighbors
        conn *connection = current_city->connections;
        while (connection != NULL) {
            // Skip if we've already found the shortest path to this neighbor
            if (done[connection->destination->id] == NULL) {
                // Create a new path for the neighbor
                path *new_path = create_path(connection->destination, current_city, current_path->dist + connection->distance);
                enqueue(pq, new_path);
            }
            connection = connection->next;
        }
    }
    
    // Free the priority queue
    free_priority_queue(pq);
    return done;
}

// Find the shortest path using Dijkstra's algorithm
int dijkstra_search(city *from, city *to, int num_cities) {
    if (from == to) return 0;
    
    path **done = dijkstra(from, to, num_cities);
    
    // If no path was found
    if (done[to->id] == NULL) {
        free(done);
        return -1;
    }
    
    // Get the shortest distance
    int distance = done[to->id]->dist;
    
    // Count cities involved in the search
    int cities_involved = 0;
    for (int i = 0; i < num_cities; i++) {
        if (done[i] != NULL) {
            cities_involved++;
        }
    }
    
    printf("Cities involved in the search: %d\n", cities_involved);
    
    // Print the path
    printf("Path: ");
    city *current = to;
    city *path[MAX_CITIES];
    int path_length = 0;
    
    while (current != NULL) {
        path[path_length++] = current;
        
        if (current == from) {
            break;
        }
        
        // Find the previous city in the path
        current = done[current->id]->prev;
    }
    
    // Print in reverse order (from source to destination)
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", path[i]->name);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
    
    // Free the done array
    for (int i = 0; i < num_cities; i++) {
        if (done[i] != NULL) {
            free(done[i]);
        }
    }
    free(done);
    
    return distance;
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

long nano_seconds(struct timespec *start_time, struct timespec *end_time) {
    return (end_time->tv_nsec - start_time->tv_nsec) +
           (end_time->tv_sec - start_time->tv_sec) * 1000000000;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <from_city> <to_city>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    map *trains = graph();
    if (!trains) {
        fprintf(stderr, "Failed to load the graph.\n");
        return EXIT_FAILURE;
    }
    
    for (int i = 0; i < bucket_count; i++) {
        city_node *current_node = city_map->buckets[i];
        
        // Traverse the linked list in the bucket
        while (current_node != NULL) {
            printf("City Name: %s\n", current_node->city_ptr->name);
            current_node = current_node->next;
        }
    }


    city *from = lookup(trains, argv[1]);
    city *to = lookup(trains, argv[2]);
    
    if (!from || !to) {
        fprintf(stderr, "City not found.\n");
        free_graph(trains);
        return EXIT_FAILURE;
    }
    
    printf("Finding shortest path from %s to %s using Dijkstra's algorithm...\n", from->name, to->name);
    
    // Test with Dijkstra's algorithm
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int shortest_distance = dijkstra_search(from, to, trains->city_count);
    
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