#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <thread>
#include <vector>

#include "../dsu.h"
#include "../graph.h"
#include "mst_types.h"

template <typename WeightT>
struct BestEdgeCandidate {
    bool valid = false;
    Edge<WeightT> edge{};

    void update(const Edge<WeightT>& e) {
        if (!valid || e.weight < edge.weight ||
            (e.weight == edge.weight && e.id < edge.id)) {
            edge = e;
            valid = true;
        }
    }
};

template <typename WeightT>
MSTResult<WeightT> boruvka_mst(
    const Graph<WeightT>& g,
    std::size_t thread_count = std::thread::hardware_concurrency())
{
    MSTResult<WeightT> result;

    const NodeId n = g.num_vertices();
    const auto& edges = g.get_edges();

    if (n == 0) {
        result.connected = true;
        return result;
    }

    if (thread_count == 0) {
        thread_count = 1;
    }

    if (n == 1) {
        result.connected = true;
        return result;
    }

    if (edges.empty()) {
        result.connected = false;
        return result;
    }

    DSU<NodeId> dsu(n);
    std::size_t components = n;
    result.edges.reserve(n - 1);

    const std::size_t m = edges.size();

    // We keep these arrays indexed by vertex id / current component root id.
    std::vector<NodeId> comp_of_vertex(n);
    std::vector<char> is_root(n, 0);
    std::vector<NodeId> roots;

    while (components > 1) {
        // Snapshot the current component id for every vertex.
        for (NodeId v = 0; v < n; ++v) {
            comp_of_vertex[v] = dsu.find(v);
        }

        // Collect the active component roots for this round.
        roots.clear();
        std::fill(is_root.begin(), is_root.end(), 0);

        for (NodeId v = 0; v < n; ++v) {
            const NodeId r = comp_of_vertex[v];
            if (!is_root[r]) {
                is_root[r] = 1;
                roots.push_back(r);
            }
        }

        // No active roots means something is inconsistent, but just stop safely.
        if (roots.empty()) {
            break;
        }

        const std::size_t tcount = std::min<std::size_t>(thread_count, std::max<std::size_t>(1, m));
        std::vector<std::vector<BestEdgeCandidate<WeightT>>> local_best(
            tcount, std::vector<BestEdgeCandidate<WeightT>>(n));

        // Scan edges in parallel.
        std::vector<std::thread> workers;
        workers.reserve(tcount);

        for (std::size_t t = 0; t < tcount; ++t) {
            const std::size_t left = (t * m) / tcount;
            const std::size_t right = ((t + 1) * m) / tcount;

            workers.emplace_back([&, t, left, right] {
                auto& best = local_best[t];

                for (std::size_t i = left; i < right; ++i) {
                    const auto& e = edges[i];

                    const NodeId ru = comp_of_vertex[e.u];
                    const NodeId rv = comp_of_vertex[e.v];

                    if (ru == rv) {
                        continue; // internal edge, not useful this round
                    }

                    best[ru].update(e);
                    best[rv].update(e);
                }
            });
        }

        for (auto& th : workers) {
            th.join();
        }

        // Merge local answers into one best outgoing edge per component.
        std::vector<BestEdgeCandidate<WeightT>> best(n);

        for (const NodeId r : roots) {
            for (std::size_t t = 0; t < tcount; ++t) {
                if (local_best[t][r].valid) {
                    best[r].update(local_best[t][r].edge);
                }
            }
        }

        // Union the chosen edges. This is done sequentially for simplicity.
        bool merged_any = false;

        for (const NodeId r : roots) {
            if (!best[r].valid) {
                continue;
            }

            const auto& e = best[r].edge;
            if (dsu.unite(e.u, e.v)) {
                result.edges.push_back(e);
                result.total_weight += e.weight;
                --components;
                merged_any = true;

                if (components == 1) {
                    break;
                }
            }
        }

        // If nothing merged, the graph is disconnected.
        if (!merged_any) {
            break;
        }
    }

    result.connected = (components == 1);
    return result;
}
