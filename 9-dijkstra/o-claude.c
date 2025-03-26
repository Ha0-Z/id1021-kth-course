#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Structure to represent a connection between cities
typedef struct connection {
    struct city *city;  // Pointer to the connected city
    int weight;         // Time in minutes to travel to this city
    struct connection *next;  // Pointer to next connection
} connection;

// Structure to represent a city
typedef struct city {
    char *name;
    int id;
    connection *neighbors;  // Linked list of connections
} city;

// Structure to represent a path
typedef struct path {
    city *city;
    struct city *prev;
    int dist;
} path;

// Structure for the priority queue node
typedef struct pq_node {
    path *path;
    struct pq_node *next;
} pq_node;

// Structure for the priority queue
typedef struct {
    pq_node *head;
} priority_queue;

// Function to create a new city
city* create_city(char *name, int id) {
    city *c = (city*)malloc(sizeof(city));
    c->name = strdup(name);
    c->id = id;
    c->neighbors = NULL;
    return c;
}

// Function to add a connection between cities
void add_connection(city *from, city *to, int weight) {
    connection *conn = (connection*)malloc(sizeof(connection));
    conn->city = to;
    conn->weight = weight;
    conn->next = from->neighbors;
    from->neighbors = conn;
}

// Function to create a new path
path* create_path(city *city, struct city *prev, int dist) {
    path *p = (path*)malloc(sizeof(path));
    p->city = city;
    p->prev = prev;
    p->dist = dist;
    return p;
}

// Function to create a new priority queue
priority_queue* create_priority_queue() {
    priority_queue *pq = (priority_queue*)malloc(sizeof(priority_queue));
    pq->head = NULL;
    return pq;
}

// Function to add a path to the priority queue (ordered by distance)
void enqueue(priority_queue *pq, path *p) {
    pq_node *new_node = (pq_node*)malloc(sizeof(pq_node));
    new_node->path = p;
    
    // If the queue is empty or the new path has a smaller distance than the head
    if (pq->head == NULL || p->dist < pq->head->path->dist) {
        new_node->next = pq->head;
        pq->head = new_node;
    } else {
        pq_node *current = pq->head;
        while (current->next != NULL && current->next->path->dist <= p->dist) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// Function to remove and return the path with the smallest distance
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

// Function to check if the priority queue is empty
int is_empty(priority_queue *pq) {
    return pq->head == NULL;
}

// Function to free the priority queue
void free_priority_queue(priority_queue *pq) {
    while (!is_empty(pq)) {
        path *p = dequeue(pq);
        free(p);
    }
    free(pq);
}

// Hash function for city names
unsigned int hash(char *name) {
    unsigned int hash = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        hash = hash * 31 + name[i];
    }
    return hash;
}

// Hash table entry
typedef struct hash_entry {
    char *name;
    city *city;
    struct hash_entry *next;
} hash_entry;

// Hash table
typedef struct {
    hash_entry **buckets;
    int size;
} hash_table;

// Function to create a hash table
hash_table* create_hash_table(int size) {
    hash_table *ht = (hash_table*)malloc(sizeof(hash_table));
    ht->size = size;
    ht->buckets = (hash_entry**)calloc(size, sizeof(hash_entry*));
    return ht;
}

// Function to insert a city into the hash table
void hash_insert(hash_table *ht, char *name, city *c) {
    unsigned int index = hash(name) % ht->size;
    hash_entry *entry = (hash_entry*)malloc(sizeof(hash_entry));
    entry->name = strdup(name);
    entry->city = c;
    entry->next = ht->buckets[index];
    ht->buckets[index] = entry;
}

// Function to lookup a city in the hash table
city* hash_lookup(hash_table *ht, char *name) {
    unsigned int index = hash(name) % ht->size;
    hash_entry *entry = ht->buckets[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry->city;
        }
        entry = entry->next;
    }
    return NULL;
}

// Function to free the hash table
void free_hash_table(hash_table *ht) {
    for (int i = 0; i < ht->size; i++) {
        hash_entry *entry = ht->buckets[i];
        while (entry != NULL) {
            hash_entry *temp = entry;
            entry = entry->next;
            free(temp->name);
            free(temp);
        }
    }
    free(ht->buckets);
    free(ht);
}

// Dijkstra's algorithm implementation
path** dijkstra(city *src, city *dest, int num_cities) {
    // Initialize done array to keep track of shortest paths
    path **done = (path**)calloc(num_cities, sizeof(path*));
    
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
            // Free the priority queue and return the done array
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
        connection *conn = current_city->neighbors;
        while (conn != NULL) {
            // Skip if we've already found the shortest path to this neighbor
            if (done[conn->city->id] == NULL) {
                // Create a new path for the neighbor
                path *new_path = create_path(conn->city, current_city, current_path->dist + conn->weight);
                enqueue(pq, new_path);
            }
            conn = conn->next;
        }
    }
    
    // Free the priority queue
    free_priority_queue(pq);
    return done;
}

// Function to print the shortest path
void print_path(path **done, city *dest) {
    if (done[dest->id] == NULL) {
        printf("No path found to %s\n", dest->name);
        return;
    }
    
    // Trace back the path
    printf("Shortest path to %s (total time: %d minutes):\n", dest->name, done[dest->id]->dist);
    
    // Create an array to store the path
    int max_path_length = 100;  // Assuming no path is longer than 100 cities
    city **path_cities = (city**)malloc(max_path_length * sizeof(city*));
    int path_length = 0;
    
    // Trace back the path
    city *current = dest;
    while (current != NULL) {
        path_cities[path_length++] = current;
        current = done[current->id]->prev;
    }
    
    // Print the path in reverse order (from source to destination)
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", path_cities[i]->name);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
    
    free(path_cities);
}

// Function to free the done array
void free_done(path **done, int num_cities) {
    for (int i = 0; i < num_cities; i++) {
        if (done[i] != NULL) {
            free(done[i]);
        }
    }
    free(done);
}

// Function to free the cities and connections
void free_cities(city **cities, int num_cities) {
    for (int i = 0; i < num_cities; i++) {
        connection *conn = cities[i]->neighbors;
        while (conn != NULL) {
            connection *temp = conn;
            conn = conn->next;
            free(temp);
        }
        free(cities[i]->name);
        free(cities[i]);
    }
    free(cities);
}

// Main function to demonstrate the algorithm
int main() {
    // Number of cities
    int num_cities = 5;
    
    // Create cities
    city **cities = (city**)malloc(num_cities * sizeof(city*));
    cities[0] = create_city("Malmo", 0);
    cities[1] = create_city("Stockholm", 1);
    cities[2] = create_city("Gothenburg", 2);
    cities[3] = create_city("Jonkoping", 3);
    cities[4] = create_city("Sodertalje", 4);
    
    // Create hash table for city lookup
    hash_table *ht = create_hash_table(num_cities * 2);
    for (int i = 0; i < num_cities; i++) {
        hash_insert(ht, cities[i]->name, cities[i]);
    }
    
    // Add connections (bidirectional)
    add_connection(cities[0], cities[3], 270);  // Malmo -> Jonkoping
    add_connection(cities[3], cities[0], 270);  // Jonkoping -> Malmo
    
    add_connection(cities[3], cities[2], 150);  // Jonkoping -> Gothenburg
    add_connection(cities[2], cities[3], 150);  // Gothenburg -> Jonkoping
    
    add_connection(cities[3], cities[1], 320);  // Jonkoping -> Stockholm
    add_connection(cities[1], cities[3], 320);  // Stockholm -> Jonkoping
    
    add_connection(cities[2], cities[1], 420);  // Gothenburg -> Stockholm
    add_connection(cities[1], cities[2], 420);  // Stockholm -> Gothenburg
    
    add_connection(cities[1], cities[4], 30);   // Stockholm -> Sodertalje
    add_connection(cities[4], cities[1], 30);   // Sodertalje -> Stockholm
    
    add_connection(cities[4], cities[3], 290);  // Sodertalje -> Jonkoping
    add_connection(cities[3], cities[4], 290);  // Jonkoping -> Sodertalje
    
    // Find shortest path from Malmo to Stockholm
    printf("Finding shortest path from Malmo to Stockholm...\n");
    path **done = dijkstra(cities[0], cities[1], num_cities);
    
    // Print the path
    print_path(done, cities[1]);
    
    // Print the number of cities involved in the search
    int cities_involved = 0;
    for (int i = 0; i < num_cities; i++) {
        if (done[i] != NULL) {
            cities_involved++;
        }
    }
    printf("Number of cities involved in the search: %d\n", cities_involved);
    
    // Free memory
    free_done(done, num_cities);
    free_hash_table(ht);
    free_cities(cities, num_cities);
    
    return 0;
}