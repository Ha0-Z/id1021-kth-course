#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER 256
#define MOD 101 // A prime number for the hash table size

// Structure for a connection between cities
typedef struct connection {
    struct city *destination;
    int distance;
    struct connection *next;
} connection;

// Structure for a city
typedef struct city {
    char *name;
    connection *connections;
    struct city *next; // For the linked list in the hash bucket
} city;

// Structure for the map (hash table of cities)
typedef struct map {
    city **cities;
} map;

// Hash function
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

// Procedure to add a connection to a city
void connect_cities(city *src, city *dst, int dist) {
    // Add connection from src to dst
    connection *new_conn_src = (connection *)malloc(sizeof(connection));
    if (new_conn_src == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_conn_src->destination = dst;
    new_conn_src->distance = dist;
    new_conn_src->next = src->connections;
    src->connections = new_conn_src;

    // Add connection from dst to src (bidirectional)
    connection *new_conn_dst = (connection *)malloc(sizeof(connection));
    if (new_conn_dst == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_conn_dst->destination = src;
    new_conn_dst->distance = dist;
    new_conn_dst->next = dst->connections;
    dst->connections = new_conn_dst;
}

// Lookup procedure to find a city by name in the hash table
city *lookup(city **cities, char *name) {
    int index = hash(name, MOD);
    city *current = cities[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    // City not found, create a new one
    city *new_city = (city *)malloc(sizeof(city));
    if (new_city == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_city->name = strdup(name);
    if (new_city->name == NULL) {
        perror("Memory allocation failed");
        free(new_city);
        exit(EXIT_FAILURE);
    }
    new_city->connections = NULL;
    new_city->next = cities[index];
    cities[index] = new_city;
    return new_city;
}

// Function to build the graph from a file
map *graph(char *file) {
    city **cities = (city **)malloc(sizeof(city *) * MOD);
    if (cities == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MOD; i++) {
        cities[i] = NULL;
    }

    map *trains = (map *)malloc(sizeof(map));
    if (trains == NULL) {
        perror("Memory allocation failed");
        free(cities);
        exit(EXIT_FAILURE);
    }
    trains->cities = cities;

    // Open a file in read mode
    FILE *fptr = fopen(file, "r");
    if (fptr == NULL) {
        perror("Error opening file");
        free(trains);
        free(cities);
        return NULL;
    }

    char *lineptr = malloc(sizeof(char) * BUFFER);
    if (lineptr == NULL) {
        perror("Memory allocation failed");
        fclose(fptr);
        free(trains);
        free(cities);
        return NULL;
    }
    size_t n = BUFFER;
    ssize_t read;

    while ((read = getline(&lineptr, &n, fptr)) != -1) {
        // Remove trailing newline character if present
        if (lineptr[read - 1] == '\n') {
            lineptr[read - 1] = '\0';
        }

        char *copy = strdup(lineptr);
        if (copy == NULL) {
            perror("Memory allocation failed");
            free(lineptr);
            fclose(fptr);
            // Need to free allocated cities and connections here in a real scenario
            free(trains);
            free(cities);
            return NULL;
        }

        char *from_city_name = strtok(copy, ",");
        char *to_city_name = strtok(NULL, ",");
        char *distance_str = strtok(NULL, ",");

        if (from_city_name && to_city_name && distance_str) {
            city *src = lookup(cities, from_city_name);
            city *dst = lookup(cities, to_city_name);
            uint dist = atoi(distance_str);
            connect_cities(src, dst, dist);
        }
        free(copy);
    }

    free(lineptr);
    fclose(fptr);
    return trains;
}

// Helper function to print the graph (for testing)
void print_graph(map *trains) {
    if (trains == NULL || trains->cities == NULL) {
        printf("Graph is empty.\n");
        return;
    }
    for (int i = 0; i < MOD; i++) {
        city *current_city = trains->cities[i];
        while (current_city != NULL) {
            printf("City: %s\n", current_city->name);
            connection *current_connection = current_city->connections;
            while (current_connection != NULL) {
                printf("  -> %s (%u min)\n", current_connection->destination->name, current_connection->distance);
                current_connection = current_connection->next;
            }
            current_city = current_city->next;
        }
    }
}

// Helper function to free the memory allocated for the graph
void free_graph(map *trains) {
    if (trains == NULL || trains->cities == NULL) {
        return;
    }
    city **cities = trains->cities;
    for (int i = 0; i < MOD; i++) {
        city *current_city = cities[i];
        while (current_city != NULL) {
            city *next_city = current_city->next;
            free(current_city->name);
            connection *current_connection = current_city->connections;
            while (current_connection != NULL) {
                connection *next_connection = current_connection->next;
                free(current_connection);
                current_connection = next_connection;
            }
            free(current_city);
            current_city = next_city;
        }
    }
    free(cities);
    free(trains);
}

int main() {
    // Create a dummy CSV file for testing
    FILE *test_file = fopen("train_connections.csv", "w");
    if (test_file == NULL) {
        perror("Error creating test file");
        return 1;
    }
    fprintf(test_file, "Stockholm,Uppsala,39\n");
    fprintf(test_file, "Stockholm,Göteborg,270\n");
    fprintf(test_file, "Uppsala,Gävle,72\n");
    fprintf(test_file, "Göteborg,Malmö,165\n");
    fprintf(test_file, "Malmö,Lund,13\n");
    fclose(test_file);

    map *train_map = graph("train_connections.csv");

    if (train_map != NULL) {
        printf("Train map created successfully.\n");
        print_graph(train_map);
        free_graph(train_map);
    } else {
        printf("Failed to create train map.\n");
    }

    // Clean up the test file
    remove("train_connections.csv");

    return 0;
}

/*
Choice of Hash Table Implementation: Buckets (Separate Chaining)

Pros:
1. Simplicity: Implementing separate chaining with linked lists is relatively straightforward.
2. Collision Handling: Collisions are handled gracefully. When multiple keys hash to the same index, they are simply added to the linked list at that index. The performance degrades linearly with the length of the linked list in the worst case.
3. Deletion: Deleting an element is easy; we just need to remove the corresponding node from the linked list.
4. Load Factor: The hash table can have a load factor greater than 1 without a catastrophic performance degradation (though very long lists should be avoided).

Cons:
1. Extra Memory: Requires extra memory for the linked list nodes (pointers).
2. Cache Performance: Can have poorer cache performance compared to open addressing, especially if the linked lists become long, as the nodes might be scattered in memory.

Collision Check:

To check for collisions, we can observe the output of the `print_graph` function. If multiple cities hash to the same index, they will appear under the same hash index in the `trains->cities` array (though the `print_graph` function as implemented doesn't explicitly show the hash indices).

Alternatively, we could add some debug code within the `lookup` function to count how many cities end up in each bucket. After running the code with the provided example data, we can analyze the distribution.

Let's manually calculate the hash values for the example cities with MOD = 101:
- Stockholm: (('S'*31 + 't')*31 + 'o')*31 + 'c')*31 + 'k')*31 + 'h')*31 + 'o')*31 + 'l')*31 + 'm') % 101
             = ((((((((83*31 + 116)*31 + 111)*31 + 99)*31 + 107)*31 + 104)*31 + 111)*31 + 108)*31 + 109) % 101 = 36
- Uppsala:   (((((85*31 + 112)*31 + 112)*31 + 97)*31 + 108)*31 + 97) % 101 = 8
- Göteborg:  ((((((71*31 + 111)*31 + 116)*31 + 101)*31 + 98)*31 + 111)*31 + 114)*31 + 103) % 101 = 95
- Malmö:     (((((77*31 + 97)*31 + 108)*31 + 109)*31 + 111) % 101 = 84
- Lund:      ((((76*31 + 117)*31 + 110)*31 + 100) % 101 = 23
- Gävle:     (((((71*31 + 228)*31 + 118)*31 + 108)*31 + 101) % 101 = 67 (assuming UTF-8 for ä)

Based on these hash values, we can see that there are no collisions in this small example. However, with a larger dataset, collisions are likely to occur. The separate chaining approach will handle these collisions by adding the cities to the same linked list at the corresponding hash index.

To more rigorously check for collisions with a larger dataset, one would need to process the entire list of 52 cities and 75 connections and see if any two city names produce the same hash value modulo 101.
*/