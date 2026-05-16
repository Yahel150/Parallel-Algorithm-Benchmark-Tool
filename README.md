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









