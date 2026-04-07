#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "min_heap.h"
#include "../graph.h"
#include "mst_types.h"


template <typename WeightT>
MSTResult<WeightT> prim_mst(const Graph<WeightT>& g) {
    if (g.isDirected())
        throw std::invalid_argument("prim_mst expects an undirected graph");

    MSTResult<WeightT> result;
    const NodeId n = g.num_vertices();
    if (n == 0) return result;

    // Build local adjacency that includes edge IDs.
    // This replaces the O(E) hash-map build + per-edge hash lookups.
    struct AdjEntry { NodeId v; WeightT w; std::uint64_t eid; };
    std::vector<std::vector<AdjEntry>> adj(n);
    for (const auto& e : g.get_edges()) {
        adj[e.u].push_back({e.v, e.weight, e.id});
        adj[e.v].push_back({e.u, e.weight, e.id});
    }

    const WeightT INF = std::numeric_limits<WeightT>::max();

    std::vector<char>        in_mst(n, 0);
    std::vector<WeightT>     key(n, INF);
    std::vector<NodeId>      parent(n, n);
    std::vector<std::uint64_t> parent_eid(n, 0);  // edge ID that gave current best key

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
                result.edges.emplace_back(parent[u], u, cur_key, parent_eid[u]);
                result.total_weight += cur_key;
            }

            for (const auto& [v, w, eid] : adj[u]) {
                if (in_mst[v]) continue;

                if (w < key[v]) {
                    key[v]        = w;
                    parent[v]     = u;
                    parent_eid[v] = eid;

                    if (pq.contains(v)) pq.decrease_key(v, w);
                    else                pq.push(v, w);
                }
            }
        }
    }

    result.connected = (result.edges.size() == n - 1);
    return result;
}