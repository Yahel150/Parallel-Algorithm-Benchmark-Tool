#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>


#include "../graph.h"
#include "shortestpaths_types.h"

template <typename WeightT>
ShortestPathResult<WeightT> dijkstra_shortest_path(const Graph<WeightT>& g, NodeId source) {
    if (source >= g.num_vertices()) {
        throw std::invalid_argument("dijkstra_shortest_path: source is out of range");
    }

    ShortestPathResult<WeightT> result;
    const NodeId n = g.num_vertices();
    result.source = source;
    result.dist.assign(n, std::numeric_limits<WeightT>::max());
    result.parent.assign(n, source);

    if (n == 0) {
        return result;
    }

    const auto& adj = g.get_adjacency();
    const auto& edges = g.get_edges();

    // Dijkstra assumes non-negative weights.
    for (const auto& e : edges) {
        if (e.weight < WeightT{}) {
            throw std::invalid_argument("dijkstra_shortest_path: negative edge weight found");
        }
    }

    using PQItem = std::pair<WeightT, NodeId>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;

    const WeightT INF = std::numeric_limits<WeightT>::max();
    result.dist[source] = WeightT{};
    result.parent[source] = source;
    pq.emplace(WeightT{}, source);

    while (!pq.empty()) {
        const PQItem top = pq.top(); pq.pop();
        const WeightT du = top.first;
        const NodeId u = top.second;

        if (du != result.dist[u]) {
            continue;
        }

        for (const auto& edge : adj[u]) {
            const NodeId v = edge.first;
            const WeightT w = edge.second;
            if (result.dist[u] == INF) {
                continue;
            }

            // Avoid overflow when using unsigned or bounded integer weights.
            if (w > INF - result.dist[u]) {
                continue;
            }

            const WeightT cand = static_cast<WeightT>(result.dist[u] + w);
            if (cand < result.dist[v]) {
                result.dist[v] = cand;
                result.parent[v] = u;
                pq.emplace(cand, v);
            }
        }
    }

    return result;
}
