#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../graph.h"
#include "shortestpaths_types.h"

template <typename WeightT>
ShortestPathResult<WeightT> bellman_ford_shortest_path(const Graph<WeightT>& g, NodeId source) {
    if (source >= g.num_vertices()) {
        throw std::invalid_argument("bellman_ford_shortest_path: source is out of range");
    }

    ShortestPathResult<WeightT> result;
    const NodeId n = g.num_vertices();
    result.source = source;
    result.dist.assign(n, std::numeric_limits<WeightT>::max());
    result.parent.assign(n, source);

    if (n == 0) {
        return result;
    }

    const WeightT INF = std::numeric_limits<WeightT>::max();
    const auto& adj = g.get_adjacency();

    result.dist[source] = WeightT{};
    result.parent[source] = source;

    // Relax all edges up to n-1 times.
    for (NodeId iter = 0; iter + 1 < n; ++iter) {
        bool changed = false;

        for (NodeId u = 0; u < n; ++u) {
            if (result.dist[u] == INF) {
                continue;
            }

            for (const auto& edge : adj[u]) {
                const NodeId v = edge.first;
                const WeightT w = edge.second;
                if (result.dist[u] != INF && result.dist[u] <= INF - w) {
                    const WeightT cand = static_cast<WeightT>(result.dist[u] + w);
                    if (cand < result.dist[v]) {
                        result.dist[v] = cand;
                        result.parent[v] = u;
                        changed = true;
                    }
                }
            }
        }

        if (!changed) {
            break;
        }
    }

    // Detect a negative cycle reachable from source.
    for (NodeId u = 0; u < n; ++u) {
        if (result.dist[u] == INF) {
            continue;
        }

        for (const auto& edge : adj[u]) {
            const NodeId v = edge.first;
            const WeightT w = edge.second;
            if (result.dist[u] != INF && result.dist[u] <= INF - w) {
                const WeightT cand = static_cast<WeightT>(result.dist[u] + w);
                if (cand < result.dist[v]) {
                    result.has_negative_cycle = true;
                    return result;
                }
            }
        }
    }

    return result;
}
