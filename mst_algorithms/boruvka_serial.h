// mst_algorithms/boruvka_serial.h
#pragma once

#include <stdexcept>
#include <vector>

#include "../dsu.h"
#include "../graph.h"
#include "boruvka.h"
#include "mst_types.h"

template <typename WeightT>
MSTResult<WeightT> boruvka_serial_mst(const Graph<WeightT>& g) {
    if (g.isDirected()) {
        throw std::invalid_argument("boruvka_serial_mst expects an undirected graph");
    }

    MSTResult<WeightT> result;
    const NodeId n = g.num_vertices();
    if (n == 0) {
        return result;
    }

    const auto& edges = g.get_edges();
    if (edges.empty()) {
        result.connected = (n <= 1);
        return result;
    }

    DSU<NodeId> dsu(n);
    std::size_t components = n;
    result.edges.reserve(n - 1);

    std::vector<NodeId> comp_of_vertex(n);
    std::vector<char> is_root(n, 0);
    std::vector<NodeId> roots;

    while (components > 1) {
        for (NodeId v = 0; v < n; ++v) {
            comp_of_vertex[v] = dsu.find(v);
        }

        roots.clear();
        std::fill(is_root.begin(), is_root.end(), 0);

        for (NodeId v = 0; v < n; ++v) {
            const NodeId root = comp_of_vertex[v];
            if (!is_root[root]) {
                is_root[root] = 1;
                roots.push_back(root);
            }
        }

        if (roots.empty()) {
            break;
        }

        std::vector<BestEdgeCandidate<WeightT>> best(n);

        for (const auto& e : edges) {
            const NodeId ru = comp_of_vertex[e.u];
            const NodeId rv = comp_of_vertex[e.v];

            if (ru == rv) {
                continue;
            }

            best[ru].update(e);
            best[rv].update(e);
        }

        std::size_t added_this_round = 0;
        for (const NodeId root : roots) {
            if (!best[root].valid) {
                continue;
            }

            const auto& e = best[root].edge;
            if (dsu.unite(e.u, e.v)) {
                result.edges.push_back(e);
                result.total_weight += e.weight;
                --components;
                ++added_this_round;

                if (components == 1) {
                    break;
                }
            }
        }

        if (added_this_round == 0) {
            result.connected = false;
            break;
        }
    }

    result.connected = (components == 1);
    return result;
}
