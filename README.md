# Parallel Algorithms Benchmark Tool

This project implements parallel algorithms, inspired by research papers, for three classic problem families: Minimum Spanning Tree (MST), Shortest Path, and Sorting. It also provides a benchmarking tool designed to compare these parallel algorithms against their classical counterparts on large inputs, in order to evaluate their performance, scalability, and practical advantages.

## How benchmarks are run and compared

We checked the performence for large and dense graphs and arrays
Default suite: 25 cases per problem (see app/benchmark_config.h):
  MST: undirected connected graphs. 10k–60k vertices, 8×–32× average edge factors.
  Shortest path: directed connected graphs. 2k–10k vertices, 8×–32× average edge factors.
  Sort: arrays from 100k up to 5M elements, full 32-bit int range.
  
### Demonstrations

### MST Demonstration:

https://github.com/user-attachments/assets/8ec1668d-97d1-439c-b800-02fa7c407d15

//

//

//

### Shortest Path Demonstration:

https://github.com/user-attachments/assets/de3a74af-c489-4a68-8a21-a859c462d689

//

//

//

### Sorting Demonstration

https://github.com/user-attachments/assets/285c855b-8655-4790-9544-467bb9deaa6f






## Parallel implementations

### Parallel Borůvka (MST)
**Classical idea**: Borůvka’s algorithm (1920s) grows an MST by repeating rounds: for each connected component, pick the cheapest edge that leaves that component, then add those edges and merge components until one component remains.
the components are maintained with a disjoint-set(union-find)
**How we parrallize it**:
The heavy work is scanning all edges to compute, for each component root, the best outgoing edge. Our implementation splits the edge list across thread workers; each thread maintains local “best edge per component root” arrays.
Reducing the the scan of all edges every iteration, Potentially improve the performence by the number of threads
**Results and explanations**
<img width="554" height="209" alt="image" src="https://github.com/user-attachments/assets/127f9106-98fc-4c9d-8f47-8f6e523de504" />



