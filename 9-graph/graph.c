#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <stdint.h> // For uint
#include <sys/time.h> // For gettimeofday

#define BUFFER 1024
#define MOD 101 // A prime number for hash table size

// Structure to represent a connection to another city
typedef struct connection {
  struct city *destination;
  int distance;
  struct connection *next;
} conn;

// Structure to represent a city
typedef struct city {
  wchar_t *name;
  conn *connections;
} city;

// Structure to represent the map (using a hash table of cities)
typedef struct map {
    city **cities;
} map;

// Function to add a connection between two cities
void connect(city *src, city *dst, int distance) {
  // Add connection from src to dst
  conn *new_conn_src = (conn*)malloc(sizeof(conn));

  new_conn_src->destination = dst;
  new_conn_src->distance = distance;
  new_conn_src->next = src->connections;
  src->connections = new_conn_src;

  // Add connection from dst to src (bidirectional)
  conn *new_conn_dst = (conn*)malloc(sizeof(conn));

  new_conn_dst->destination = src;
  new_conn_dst->distance = distance;
  new_conn_dst->next = dst->connections;
  dst->connections = new_conn_dst;
}

// Structure to represent a node in the hash table bucket
typedef struct city_node {
  city *city_ptr;
  struct city_node *next;
} city_node;

// Structure to represent the map (using a hash table of city nodes)
typedef struct map {
  city_node **buckets;
} map;

// Hash function to generate a hash value for a city name
int hash(wchar_t *name, int mod) {
  int h = 0;
  int i = 0;
  wchar_t c = 0;
  while ((c = name[i]) != 0) {
      h = (h * 31 + c) % mod;
      i++;
  }
  return h;
}

// Function to lookup a city in the hash table. Creates a new city if not found.
city* lookup(map *trains, char *name_mb) {
  wchar_t name_wc[BUFFER]; // Use a buffer for conversion
  mbstate_t ps;
  memset(&ps, 0, sizeof(ps)); // Initialize conversion state

  size_t res = mbsrtowcs(name_wc, (const char**)&name_mb, BUFFER - 1, &ps);
  if (res == (size_t)-1) {
      perror("mbsrtowcs failed");
      return NULL;
  }
  name_wc[res] = 0; // Null-terminate the wide character string

  int index = hash_w(name_wc, MOD);
  city_node *current = trains->buckets[index];

  while (current != NULL) {
      if (wcscmp(current->city_ptr->name, name_wc) == 0) {
          return current->city_ptr;
      }
      current = current->next;
  }

  city *new_city = (city*)malloc(sizeof(city));
  if (new_city == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  new_city->name = wcsdup(name_wc); // Duplicate the wide character string
  new_city->connections = NULL;

  city_node *new_node = (city_node*)malloc(sizeof(city_node));
  if (new_node == NULL) {
      perror("malloc failed");
      free(new_city);
      exit(EXIT_FAILURE);
  }
  new_node->city_ptr = new_city;
  new_node->next = trains->buckets[index];
  trains->buckets[index] = new_node;

  return new_city;
}

// Function to lookup a city in the hash table. Creates a new city if not found.
city* lookup(city **cities, char *name) {
  int index = hash(name, MOD);
  city *current = cities[index];

  // Search for the city in the linked list at this hash index
  while (current != NULL) {
      if (strcmp(current->name, name) == 0) {
          return current; // City found
      }
      current = current->connections; // In our hash table, we'll store cities directly, so this should be next in the hash bucket's linked list
  }

  // City not found, create a new one
  city *new_city = (city*)malloc(sizeof(city));
  if (new_city == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  new_city->name = strdup(name); // Duplicate the name
  new_city->connections = NULL;

  // Add the new city to the beginning of the linked list at this hash index
  new_city->connections = cities[index];
  cities[index] = new_city;

  return new_city;
}

// Function to build the graph from the CSV file
map* graph(char *file) {
  map *trains = (map*)malloc(sizeof(map));
  if (trains == NULL) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  trains->buckets = (city_node**)malloc(sizeof(city_node*) * MOD);
  if (trains->buckets == NULL) {
      perror("malloc failed");
      free(trains);
      exit(EXIT_FAILURE);
  }
  for (int i = 0; i < MOD; i++) {
      trains->buckets[i] = NULL;
  }

  FILE *fptr = fopen(file, "r");
  if (fptr == NULL) {
      perror("Error opening file");
      free(trains->buckets);
      free(trains);
      return NULL;
  }

  char *lineptr = NULL;
  size_t n = 0;
  size_t read;

  while ((read = getline(&lineptr, &n, fptr)) != -1) {
      // Remove trailing newline character if present
      if (read > 0 && lineptr[read - 1] == '\n') {
          lineptr[read - 1] = '\0';
      }

      char *copy = strdup(lineptr);
      if (copy == NULL) {
          perror("strdup failed");
          free(lineptr);
          fclose(fptr);
          // Handle cleanup of already allocated graph if needed
          return trains;
      }

      char *from_name = strtok(copy, ",");
      char *to_name = strtok(NULL, ",");
      char *dist_str = strtok(NULL, ",");

      if (from_name && to_name && dist_str) {
          city *src = lookup(trains, from_name);
          city *dst = lookup(trains, to_name);
          unsigned int dist = atoi(dist_str);
          connect(src, dst, dist);
      }

      free(copy);
  }

  free(lineptr);
  fclose(fptr);
  return trains;
}

// Initial depth-first search for the shortest path (with a depth limit)
int deep_first(city *from, city *to, int left) {
  if (from == to) {
      return 0;
  }
  int sofar = -1;
  conn *nxt = from->connections;
  while (nxt != NULL) {
      if (nxt->distance <= left) {
          int d = deep_first(nxt->destination, to, left - nxt->distance);
          if (d >= 0 && (sofar == -1 || (d + nxt->distance) < sofar)) {
              sofar = (d + nxt->distance);
          }
      }
      nxt = nxt->next;
  }
  return sofar;
}

void free() {
// Free every thing...

}

void init() {

}

// Helper function to calculate the time difference in milliseconds
long long time_diff_ms(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000LL + (end.tv_usec - start.tv_usec) / 1000;
}

int main(int argc, char *argv) {
  setlocale(LC_ALL, "en_US.UTF-8");

  if (argc < 3) { // Adjusted argument count
      printf("usage: %s <trains.csv> <from_city> <to_city>\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  map *trains = graph(argv[1]);
  if (trains == NULL) {
      fprintf(stderr, "Failed to load the graph.\n");
      exit(EXIT_FAILURE);
  }

  // Convert command line arguments to wide character strings
  wchar_t from_name_wc[BUFFER];
  wchar_t to_name_wc[BUFFER];
  mbstate_t ps;
  memset(&ps, 0, sizeof(ps));

  size_t res_from = mbsrtowcs(from_name_wc, (const char**)&argv[2], BUFFER - 1, &ps);
  if (res_from == (size_t)-1) {
      perror("mbsrtowcs failed for from city");
      // Handle error
      exit(EXIT_FAILURE);
  }
  from_name_wc[res_from] = 0;

  memset(&ps, 0, sizeof(ps)); // Reset state for the next conversion
  size_t res_to = mbsrtowcs(to_name_wc, (const char**)&argv[3], BUFFER - 1, &ps);
  if (res_to == (size_t)-1) {
      perror("mbsrtowcs failed for to city");
      // Handle error
      exit(EXIT_FAILURE);
  }
  to_name_wc[res_to] = 0;

  // Now we need to find the city structures using the wide character names
  // We'll need a modified lookup function or we can temporarily convert back to char* for lookup.
  // Let's stick with char* in lookup for simplicity, but ensure the internal storage in city is wchar_t*.

  city *from = lookup(trains, argv[2]); // Keep using char* for lookup for now
  city *to = lookup(trains, argv[3]);   // The lookup function now handles the conversion

  if (from == NULL || to == NULL) {
      fprintf(stderr, "City not found.\n");
      // Free the graph if needed
      exit(EXIT_FAILURE);
  }

  struct timeval t_start, t_stop;
  gettimeofday(&t_start, NULL);

  int s = search_with_dynamic_limit(from, to);

  gettimeofday(&t_stop, NULL);
  long long wall_ms = time_diff_ms(t_start, t_stop);

  if (s > 0) {
      printf("Shortest path %d found in %lldms\n", s, wall_ms);
  } else {
      printf("No path found.\n");
  }

  // ... (free graph memory) ...

  free();

  return 0;
}