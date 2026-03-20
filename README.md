# Algorithms and Data Structures 2

## Table of Contents
1. [Requirements](#requirements)
2. [Compilation Instructions](#compilation-instructions)
3. [Usage Examples](#usage-examples)
4. [Performance Metrics](#performance-metrics)
5. [Code Structure](#code-structure)
6. [Complexity Analysis](#complexity-analysis)
7. [Academic References](#academic-references)

## Requirements
- A modern C++ compiler (g++ 9.0 or higher recommended)
- CMake 3.10 or higher
- Git (for version control)

## Compilation Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/Iucasoa/Algoritmos-e-Estruturas-de-Dados-2.git
   cd Algoritmos-e-Estruturas-de-Dados-2
   ```
2. Create a build directory:
   ```bash
   mkdir build && cd build
   ```
3. Run CMake to generate the makefiles:
   ```bash
   cmake ..
   ```
4. Compile the project:
   ```bash
   make
   ```

## Usage Examples
- Example of using the bubble sort algorithm:
   ```cpp
   #include <iostream>
   #include "bubble_sort.h"
   
   int main() {
       int arr[] = {5, 3, 8, 1, 2};
       bubble_sort(arr, 5);
       // Output sorted array
       for (int i : arr) {
           std::cout << i << " ";
       }
       return 0;
   }
   ```

## Performance Metrics
- Average-case time complexity: O(n^2) for bubble sort
- Best-case time complexity: O(n) for bubble sort (when the array is already sorted)
- Space complexity: O(1)

## Code Structure
- `src/`: Contains all source code files
- `include/`: Header files
- `tests/`: Unit tests
- `docs/`: Documentation

## Complexity Analysis
1. **Time Complexity**: We analyze the time taken by different algorithms under different conditions.
2. **Space Complexity**: Discusses the memory used by algorithms.

## Academic References
- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). Introduction to Algorithms (3rd ed.). MIT Press.
- Knuth, D. E. (1998). The Art of Computer Programming, Volumes 1-4. Addison-Wesley.

---