#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
#include "../min_heap.h"
#include "../graph.h"
#include "mst_types.h"


template <typename WeightT>
MSTResult<WeightT> prim_mst(const Graph<WeightT>& g) {
    if (g.isDirected())
        throw std::invalid_argument("prim_mst expects an undirected graph");

    MSTResult<WeightT> result;
    const NodeId n = g.num_vertices();
    if (n == 0) return result;

    const WeightT INF = std::numeric_limits<WeightT>::max();
    const auto& adj = g.get_adjacency();

    std::vector<char>        in_mst(n, 0);
    std::vector<WeightT>     key(n, INF);
    std::vector<NodeId>      parent(n, n);

    IndexedMinHeap<WeightT> pq(n);
    result.edges.reserve(n - 1);

    for (NodeId start = 0; start < n; ++start) {
        if (in_mst[start]) continue;

        key[start] = WeightT{};
        parent[start] = n;
        pq.push(start, WeightT{});

        while (!pq.empty()) {
            const auto [cur_key, u] = pq.pop_min();
            in_mst[u] = 1;

            if (parent[u] != n) {
                result.edges.emplace_back(parent[u], u, cur_key, 0);
                result.total_weight += cur_key;
            }

            for (const auto& [v, w] : adj[u]) {
                if (in_mst[v]) continue;

                if (w < key[v]) {
                    key[v]        = w;
                    parent[v]     = u;

                    if (pq.contains(v)) pq.decrease_key(v, w);
                    else                pq.push(v, w);
                }
            }
        }
    }

    result.connected = (result.edges.size() == n - 1);
    return result;
}
