#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

// SEE THIS FILE FOR DECLARATIONS AND DOCUMENTATION OF SUGGESTED FUNCTIONS
#include "tsp.h"

#include "location.h"
int check_city_order(city cities[], size_t num_cities, char *cmd_cities[], size_t num_cmd_cities);
void free_cities(city cities[], size_t num_cities);



int main(int argc, char **argv)
{
  if (argc < 2){
    // printf("Error: No filename %s\n", argv[1]);
    return 1; //exit with an error if no file is provided
  }


  //open the file
  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    // printf("Error: Could not open file %s\n", argv[1]);
    return 1;
  }

  //city array to hold all city data
  city cities[100];
  size_t num_cities = sizeof(cities) /sizeof(cities[0]);

  //Read city data from file
  int cities_read = read_file(file, num_cities, cities);
  if(cities_read == 0){
    // printf("No cities in file\n");
    fclose(file);
    return 1;
  }

  //update number of cities to the actual count read
  num_cities = (size_t)cities_read;

  //close file after reading
  fclose(file);

  //print original tour from file
  // printf("Original tour: ");
  // for (size_t i = 0; i < num_cities; i++){
  //   printf("%s ", cities[i].name);
  // }
  // printf("\n");
  // return 0;
  // determine the index of the first heuristic
  size_t origin = 2; //assume heuristics start at index 2
  size_t num_heuristics = 0; // initialize it to zero first

  //identify heuristics from the command line argument
  for(size_t i = 2; i < argc; i++){
    if (argv[i][0] == '-'){
      num_heuristics ++;
    } else {
      break;
    }
  }

  origin += num_heuristics;

  //determine number of cities in the command line argument;
  size_t numcities_cmd = argc - origin;

  //if there are cities in cmd
  if (numcities_cmd > 0){
    // Check if the order of cities read from file matches the command-line arguments
    if (!check_city_order(cities, num_cities, argv + origin, numcities_cmd)) {
      // printf("Error: City order in the file does not match the command-line arguments. Exiting.\n");
      free_cities(cities, num_cities); // Free the allocated memory
      return 1;  // Exit with an error code
    }

    num_cities = numcities_cmd;//update to number of cities to command line argument
    // printf("Cities array:\n");
    // for(size_t i = 0; i < num_cities;i++){
    //   printf(" %s, %lf, %lf, %zu\n", cities[i].name,cities[i].loc.lat,cities[i].loc.lon, cities[i].index);
    // }
    // printf("\n");
  }
  city cities_copy[num_cities];
  for (size_t i = 0; i < num_cities; i++){
    cities_copy[i] = cities[i];
  }
  // printf("Cities array (copy): ");
  //   for(size_t i = 0; i < num_cities;i++){
  //     printf(" %s", cities_copy[i].name);
  //   }
  //   printf("\n");



  // the index on the command line of the origin city
  // TO DO: this is hard-coded as if there is always one method
  // after the filename; fix that to account for more than one
  // size_t origin = 3;

  // // the number of cities on the command line
  // size_t num_cities = argc - origin;

  // // TO DO: maybe add some more error checking here
  
  // // initialize names and indices of cities in the tour
  // // TO DO: read coordinates from the file and make this work for any
  // // cities on the command line (this assumes they are HVN ALB MHT BDL ORH PVD)
  // city cities[] = {
  //   {"HVN", {41.26388889, -72.88694444}, 0},
  //   {"ALB", {42.74916667, -73.80194444}, 5},
  //   {"MHT", {42.93277778, -71.43583333}, 2},
  //   {"BDL", {41.93916667, -72.68333333}, 3},
  //   {"ORH", {42.26722222, -71.87555556}, 4},
  //   {"PVD", {41.72388889, -71.42833333}, 1},
  // };

  // iterate over methods requested on command line
  for (size_t a = 2; a < 2 + num_heuristics; a++)
    {
      if (strcmp(argv[a], "-insert") == 0){
	  route_insert(num_cities, cities);
    } else if (strcmp(argv[a], "-nearest") == 0){
	  route_nearest(num_cities, cities);
    } else if (strcmp(argv[a], "-given") == 0){
      double total = calculate_total(num_cities, cities_copy);
      printf("%-10s: %12.2f", argv[a], total);
      for (size_t i = 0; i < num_cities; i++){
      printf(" %s", argv[origin + cities_copy[i].index]);
      }
      printf(" %s\n", argv[origin + cities_copy[0].index]);
      continue; //skip further processing
      
    }
    normalize_start(num_cities, cities);
    normalize_direction(num_cities, cities);

    double total = calculate_total(num_cities, cities);
    printf("%-10s: %12.2f", argv[a], total);

    for (size_t i = 0; i < num_cities; i++){
      printf(" %s", argv[origin + cities[i].index]);
      }
      printf(" %s\n", argv[origin + cities[0].index]);
      }

  // TO DO: there may be other opportunities for error checking!
  // Free all allocated memory for city names before exiting
  free_cities(cities, num_cities);
  return 0;
}

void route_insert(size_t n, city tour[])
{ 
  size_t best_orig, best_dest;
  find_closest_pair(n, tour, &best_orig, &best_dest);
  // printf("Before insertion: \n");
  // for(size_t i = 0; i < n; i++){
  //   printf("%s ", tour[i].name);
  // }
  // printf("\n");
  //make sure the two pairs are at the beginning of our array
  swap(tour, 0, best_orig);
  swap(tour, 1, best_dest);

  // printf("First 2 cities? %s and %s\n",tour[0].name, tour[1].name);
  size_t subtour_len = 2;
  while(subtour_len < n){

    size_t closest_to_tour = find_closest_to_tour(n, tour, subtour_len);
    // printf("Closest to tour? %s\n", tour[closest_to_tour].name);
    size_t best_position = find_insertion_point(n, tour, subtour_len, closest_to_tour);

    city temp = tour[subtour_len];
    //shift city to right to get room for insertion
    for(size_t i = subtour_len; i > best_position; i--){
      tour[i] = tour[i - 1];
    }
    //insert new city
    // swap(tour, best_position, closest_to_tour);
    if (closest_to_tour != subtour_len){
      tour[best_position] = tour[closest_to_tour];
      tour[closest_to_tour] = temp;
    }
    else
    tour[best_position] = temp;
 
    // printf("BEST POSITION: %ld\n", best_position);
    subtour_len++;
  }
  normalize_start(n, tour);
  normalize_direction(n, tour);
}

void find_closest_pair(size_t n, city tour[], size_t *best_orig, size_t *best_dest)
{
  double minimum_distance = INFINITY;

  for (size_t i = 0; i < n; i++){
    for (size_t j = i + 1; j < n; j++){

      double distance = location_distance(&tour[i].loc, &tour[j].loc);

      if (distance < minimum_distance){
        minimum_distance = distance;
        *best_orig = i;
        *best_dest = j;
      }
      // printf("Check if closest pair?: ");
      // printf("%s - %s: %.2f \n", tour[i].name, tour[j].name, minimum_distance);
    }
  }
  // printf("FINAL CLOSEST PAIR: ");
  // printf("%s - %s: %.2f \n", tour[*best_orig].name, tour[*best_dest].name, minimum_distance);

}

size_t find_closest_to_tour(size_t n, city tour[], size_t tour_len)
{
  double minimum_distance = INFINITY;
  size_t closest_city_index = n; //an invalid index

  for (size_t i = 0; i < n; i++){
    bool in_tour = false;

    for (size_t j = 0; j < tour_len; j++){
      if (tour[i].index == tour[j].index){ 
        in_tour = true;
        break;
      }
    }
    if (in_tour) continue; //if true then we wanna ignore everything else and keep iterating

    for (size_t k = 0; k < tour_len; k++){
      double distance = location_distance(&tour[i].loc, &tour[k].loc);
      // printf("Distance from %s to %s: %.2f\n", tour[i].name, tour[k].name, distance);
      if (distance < minimum_distance){
        minimum_distance = distance;
        closest_city_index = i;
      }
    }
  }
  // printf("closest to tour is %s: %.2f\n", tour[closest_city_index].name,minimum_distance);
  return closest_city_index;
}


size_t find_insertion_point(size_t n, city tour[], size_t subtour_len, size_t next)
{
  double minimum_increment = INFINITY;
  size_t insertion_point = 0;

  for(size_t i = 0; i < subtour_len; i++){
    double original_length = location_distance(&tour[i].loc, &tour[(i + 1) % subtour_len].loc);
    // printf("Distance between %s and %s: %.2f\n",tour[i].name, tour[(i + 1) % subtour_len].name, original_length);
    double updated_length = location_distance(&tour[i].loc, &tour[next].loc) + location_distance(&tour[next].loc, &tour[(i + 1) % subtour_len].loc);
    // printf("**Distance between %s and %s: %.2f\n", tour[i].name, tour[next].name, new1);
    // printf("**Distance between %s and %s: %.2f\n", tour[(i + 1) % subtour_len].name, tour[next].name, new2);
    // printf("Updated distance: %.2f\n", updated_length);


    double distance_increment = updated_length - original_length;
    // printf("Increment: %.2f\n", distance_increment);
    if(distance_increment < minimum_increment){
      minimum_increment = distance_increment;
      insertion_point = i + 1;
    }
  }
  // printf("FINAL INCREMENT: %.2f\n", minimum_increment);
  return insertion_point;
}

void route_nearest(size_t n, city tour[])
{
  bool visited[n]; // an array that stores booleans, to keep track of whether we have visited a city
  for (size_t i = 0; i < n; i++){
    visited[i] = false;   //setting all cities  as unvisited first
  }

  size_t curr_city = 0; //now we start at the first city(index 0)
  visited[curr_city] = true; // mark the first city as visited


  for (size_t i = 1; i < n; i++){// start at index 1 so we can go to the next city
    size_t nearest_city = find_closest_city(tour, curr_city, i, n - 1);
    
    //let's proceed to the next city
    if (nearest_city < n && !visited[nearest_city]){//means we have found something
      // printf("final nearest city: %s\n", tour[nearest_city].name);
      swap(tour, i, nearest_city); //basically adds nearest city to the tour
      visited[i] = true; //mark as visited
      curr_city = i;
    }
  }
  normalize_start(n, tour);
  normalize_direction(n,tour);
}

size_t find_closest_city(city tour[], size_t c, size_t from, size_t to)
{
  size_t nearest_city = to + 1;// initialize to invalid index
  double minimum_dist = INFINITY; //set to an arbitrarily large no. so that any value we see is less than it

  //get nearest unvisited city
  for (size_t i = from; i <= to; i++){
    if (i != c){
      double distance = location_distance(&tour[c].loc, &tour[i].loc);
      // printf("distance from %s to %s: %.2f km\n", tour[c].name, tour[i].name, distance);
      if (distance < minimum_dist){
        minimum_dist = distance;
         nearest_city = i;
        // printf("nearest_city: %s\n", tour[nearest_city].name);
      }
    }
  }
  return nearest_city;
}

double calculate_total(size_t n, city tour[])
{
  double tot = 0.0;

  for (size_t i = 0; i < n - 1; i++){
    //distance between city i and city i + 1
    tot += location_distance(&tour[i].loc, &tour[i + 1].loc); 
  }
  //add distance from last city back to first
  tot += location_distance(&tour[n - 1].loc, &tour[0].loc);
  return tot;
}

void swap(city arr[], size_t i, size_t j)
{
  if (i != j)
    {
      city temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
}

void normalize_start(size_t n, city tour[])
{ 
  size_t start_index = 0;

  for (size_t i = 0; i < n; i++){
    if (tour[i].index == 0){
      start_index = i;
      break;
  }
  }
 
  // printf("un-normalized: \n");
  // for (size_t i = 0; i < n; i++){
  //   printf("%s ", tour[i].name);
  // }
  // printf("\n");
  city normalized_tour[n];

  size_t current_index = 0;
  for (size_t i = 0; i < n; i++){
    normalized_tour[current_index++] = tour[(i + start_index) % n];
  }
  // printf("normalized: \n");
  for (size_t i = 0; i < n; i++){
    tour[i] = normalized_tour[i];
    // printf("%s ", tour[i].name);
  }
  // printf("\n");
 
}

void normalize_direction(size_t n, city tour[])
{ 
  // printf("Tour BEFORE direction normalization: \n");
  // for(size_t i = 0; i < n; i++){
  //   printf("%s ", tour[i].name);
  // }
  // printf("\n");

  if (tour[1].index > tour[n - 1].index){
    for (size_t i = 1; i <= n / 2; i++){
      swap(tour, i, n - i);
    }
  }
  
  // printf("Tour after direction normalization: \n");
  // for (size_t i = 0; i < n; i++){
    // printf("%s ", tour[i].name);
  // }
  // printf("\n");
}

int valid_city_name(const char *name){
  if (name[0] == '-') return 0;

  for (size_t i = 0; name[i] != '\0'; i++){
    if (name[i] == ',' || name[i] == '\n'){
      return 0;
    }
  }
  return 1; //valid city name
}
int valid_latitude(double latitude){
  return latitude >= -90.0 && latitude <= 90.0;
}

int valid_longitude(double longitude){
  return longitude >= -180.0 && longitude <= 180;
}

int read_file(FILE *in, size_t n, city cities[])
{
  char line[100];
  size_t city_counter = 0;
  //use fgets to read each line from file
  while(fgets(line, sizeof(line), in) && city_counter < n){
    char city_name[50];  //temporarily stores city names
    double latitude, longitude;
    

    //use sscanf to parse the line to extract city name, lat, lon
    if(sscanf(line, "%[^,],%lf,%lf", city_name, &latitude, &longitude) == 3){

      if(!valid_city_name(city_name)){
        // printf("Error: Invalid city name %s in line %s\n", city_name, line);
        continue;
      }
      if(!valid_latitude(latitude)){
        // printf("Lat out of range\n");
        continue;
      }

      if (!valid_longitude(longitude)){
        // printf("Lon out of range \n");
        continue;
      }
      //allocate memory for the city name dynamically
      char *name_copy = (char *)malloc(strlen(city_name) + 1);
      if(name_copy == NULL){
        // fprintf(stderr, "Memory allocation failed\n");
        exit(1);
      }
      strcpy(name_copy, city_name);
      // strncpy(cities[city_counter].name, city_name, sizeof(cities[city_counter].name) - 1);
      // cities[city_counter].name[sizeof(cities[city_counter].name) - 1] = '\0';

      cities[city_counter].name = name_copy;
      cities[city_counter].loc.lat = latitude;
      cities[city_counter].loc.lon = longitude;
      //set the index of the city as the current value of city_counter
      cities[city_counter].index = city_counter;

      city_counter++;
    }
    else {
      // printf("Error parsing the line\n");
      
      return 0;
    }
  }
  // printf("number of cities read: %zu\n", city_counter);
  return (int)city_counter;
}

int check_city_order(city cities[], size_t num_cities, char *cmd_cities[], size_t num_cmd_cities){
  if (num_cities != num_cmd_cities){
    // printf("Error: Number of cities in file don't match ones in cmd\n");
    return 0;
  }

  for(size_t i = 0; i < num_cities; i++){
    if (strcmp(cities[i].name, cmd_cities[i]) != 0){
      // printf("Error: City order mistmatch\n");
      return 0;
    }
  }
  return 1;
}

  // Function to free all allocated memory for city names
  void free_cities(city cities[], size_t num_cities) {
      for (size_t i = 0; i < num_cities; i++) {
          if (cities[i].name != NULL) {
            // Free the dynamically allocated memory
            free(cities[i].name); 
            // Set pointer to NULL to avoid dangling pointers
            cities[i].name = NULL; 
          }
      }
  }
