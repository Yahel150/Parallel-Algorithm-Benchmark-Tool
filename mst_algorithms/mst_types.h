#pragma once

#include <cstdint>
#include <vector>

#include "../graph.h"

template <typename WeightT>
struct MSTResult {
    std::vector<Edge<WeightT>> edges;
    WeightT total_weight = 0;
    bool connected = true;
};

namespace mst_detail {
    // Defines the total ordering used to pick the "cheapest" edge.
    // Tie-breaks by id, then endpoints, to guarantee a deterministic MST.
    template <typename WeightT>
    bool operator<(const Edge<WeightT>& a, const Edge<WeightT>& b) {
        if (a.weight != b.weight) return a.weight < b.weight;
        if (a.id != b.id) return a.id < b.id;
        if (a.u != b.u) return a.u < b.u;
        return a.v < b.v;
    }

    // Records the cheapest edge seen so far for a given component root.
    template <typename WeightT>
    void update_best_edge(char& valid, Edge<WeightT>& current, const Edge<WeightT>& candidate) {
        if (!valid || candidate.weight < current.weight ||
            (candidate.weight == current.weight && candidate.id < current.id)) {
            current = candidate;
            valid = 1;
            }
    }

}
