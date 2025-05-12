# TSP – Travelling Salesperson Problem

This project implements heuristic-based solutions for the **Travelling Salesperson Problem (TSP)** using C. It models a logistics scenario involving flying pirates, lost route maps, and the urgency of finding the shortest route between cities.

## 🏴‍☠️ Story Context

Captain Glenn’s route maps were lost at sea. Your mission is to reconstruct them by computing efficient tours between cities using different heuristics, ensuring timely delivery and avoiding walking the plank.

## 🎯 Objectives

- Process command-line arguments  
- Work with strings, arrays, and file I/O  
- Use a provided `location` library  
- Implement two TSP heuristics (`-nearest`, `-insert`) and a given-order baseline (`-given`)  

## 🧭 Heuristics

- `-given`: Use cities in the command-line order  
- `-nearest`: Greedily select the closest unvisited city  
- `-insert`: Start with the nearest pair, then insert each remaining city to minimize tour length  

## 📁 Input Format

1. **Location file** (`.dat`) with each line formatted as:  
   ```
   CITY_NAME,LATITUDE,LONGITUDE
   ```
2. Followed by:
   - One or more heuristics (`-given`, `-nearest`, `-insert`)
   - List of cities to include in the tour

## 💻 Example

```bash
./TSP ne_6.dat -given -nearest -insert HVN ALB MHT BDL ORH PVD
```

Example output:

```
-given    :       805.78 HVN ALB MHT BDL ORH PVD HVN
-nearest  :       734.16 HVN ALB MHT PVD ORH BDL HVN
-insert   :       721.47 HVN BDL ALB ORH MHT PVD HVN
```

Distances are calculated using the `location_distance` function provided in the course library.

## 🛠️ Compilation

Use the provided `Makefile` to compile the program:

```bash
make
```

This produces an executable named `TSP`.

**Note**: The `location` library files must be present in the current directory when compiling.

## 🧪 Testing Notes

- The output must match format requirements (colon alignment, city order normalization, etc.)  
- Tour equivalence is normalized by rotating and possibly reversing the city list to start at the first city  
- Your code must be **Valgrind-clean**  
- For invalid inputs: your program must **not crash or hang**, output can be anything or nothing  

## ⏱️ Performance Constraints

| Heuristic | Time Complexity | Space Complexity |
|-----------|------------------|------------------|
| `-given`  | O(n)             | O(n)             |
| `-nearest`| O(n²)            | O(n)             |
| `-insert` | O(n²)            | O(n)             |

(`n` = number of cities. `O(1)` per `location_distance`, `malloc`, and `printf`.)


This project explores practical algorithms for route optimization with constrained input and memory efficiency.
