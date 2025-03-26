#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define BUFFER 1024
#define MOD 389 // Hash table size (prime number)
#define MAX_CITIES 202 // Max number of cities for path tracking

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
    new_city->id = trains->city_count;  // Assign unique ID
    
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

// Build the graph from the CSV file
map* graph() {
    char *file = "europe.csv";
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

// Create a new priority queue
priority_queue* create_priority_queue() {
    priority_queue *pq = (priority_queue*)malloc(sizeof(priority_queue));
    if (!pq) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    pq->head = NULL;
    return pq;
}

// Add a path to the priority queue (ordered by distance)
void enqueue(priority_queue *pq, path *p) {
    pq_node *new_node = (pq_node*)malloc(sizeof(pq_node));
    if (!new_node) {
        perror("malloc failed");
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
        return NULL;
    }
    
    pq_node *temp = pq->head;
    path *p = temp->path;
    pq->head = pq->head->next;
    free(temp);
    return p;
}

// Check if the priority queue is empty
int is_empty(priority_queue *pq) {
    return pq->head == NULL;
}

// Free the priority queue
void free_priority_queue(priority_queue *pq) {
    while (!is_empty(pq)) {
        path *p = dequeue(pq);
        free(p);
    }
    free(pq);
}

// Create a new path
path* create_path(city *city, struct city *prev, int dist) {
    path *p = (path*)malloc(sizeof(path));
    if (!p) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    p->city = city;
    p->prev = prev;
    p->dist = dist;
    return p;
}

// Dijkstra's algorithm implementation
path** dijkstra(city *src, city *dest, int num_cities) {
    // Initialize done array to keep track of shortest paths
    path **done = (path**)calloc(num_cities, sizeof(path*));
    if (!done) {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize priority queue
    priority_queue *pq = create_priority_queue();
    
    // Add source city to priority queue
    path *src_path = create_path(src, NULL, 0);
    enqueue(pq, src_path);
    
    while (!is_empty(pq)) {
        // Get the path with the smallest distance
        path *current_path = dequeue(pq);
        city *current_city = current_path->city;
        
        // If we've reached the destination, we're done
        if (current_city == dest) {
            // Free the priority queue
            free_priority_queue(pq);
            return done;
        }
        
        // If we've already found the shortest path to this city, skip it
        if (done[current_city->id] != NULL) {
            free(current_path);
            continue;
        }
        
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