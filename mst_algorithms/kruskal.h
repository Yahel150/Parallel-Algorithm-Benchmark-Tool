#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../graph.h"
#include "../dsu.h"
#include "mst_types.h"

template <typename WeightT>
MSTResult<WeightT> kruskal_mst(const Graph<WeightT>& g) {
    if (g.isDirected()) {
        throw std::invalid_argument("kruskal_mst expects an undirected graph");
    }

    MSTResult<WeightT> result;
    const NodeId n = g.num_vertices();
    if (n == 0) {
        return result;
    }

    std::vector<Edge<WeightT>> edges = g.get_edges();
    std::sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
        if (a.weight != b.weight) return a.weight < b.weight;
        return a.id < b.id;
    });

    DSU<NodeId> dsu(n);
    result.edges.reserve(n > 0 ? n - 1 : 0);

    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            result.edges.push_back(e);
            result.total_weight += static_cast<std::uint64_t>(e.weight);
            if (result.edges.size() == n - 1) {
                break;
            }
        }
    }

    result.connected = (n <= 1) || (result.edges.size() == n - 1);
    return result;
}