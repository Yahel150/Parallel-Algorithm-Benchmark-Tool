# Parallel Algorithms Benchmark Tool

The project provides a command-line tool for benchmarking and comparing algorithms,
with a particular focus on demonstrating the advantages of parallel algorithms for the following problems: **Minimum Spanning Tree (MST)**, **Shortest Path**, **Sorting**

## Goals

- Present new parallel implementations for MST, shortest path, and sorting problems.
- Compare those implementations against classical algorithms.
- Provide a simple CLI for both benchmarking and viewing actual outputs.

## How benchmarks are run and compared

Default suite: 25 cases per problem (see app/benchmark_config.h):
  MST: undirected connected graphs. 10k–60k vertices, 8×–32× average edge factors.
  Shortest path: directed connected graphs. 2k–10k vertices, , 8×–32× average edge factors..
  Sort: arrays from 100k up to 5M elements, full 32-bit int range.

## Parallel implementations

### Parallel Borůvka (MST)
**Classical idea**: Borůvka’s algorithm (1920s) grows an MST by repeating rounds: for each connected component, pick the cheapest edge that leaves that component, then add those edges and merge components until one component remains.
the components are maintained with a disjoint-set(union-find)
**How we parrallize it**:
The heavy work is scanning all edges to compute, for each component root, the best outgoing edge. Our implementation splits the edge list across thread workers; each thread maintains local “best edge per component root” arrays.
Reducing the the scan of all edges every iteration, Potentially improve the performence by the number of threads
**Results and explanations**
