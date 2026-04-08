#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "../graph.h"

template <typename WeightT>
struct ShortestPathResult {
    NodeId source = 0;
    std::vector<WeightT> dist;
    std::vector<NodeId> parent;
    bool has_negative_cycle = false;
};

template <typename WeightT>
std::vector<NodeId> reconstruct_path(const ShortestPathResult<WeightT>& result, NodeId target) {
    std::vector<NodeId> path;
    if (result.has_negative_cycle) return path;
    if (target >= result.dist.size()) return path;

    const WeightT INF = std::numeric_limits<WeightT>::max();
    if (result.dist[target] == INF) return path;

    for (NodeId cur = target;; cur = result.parent[cur]) {
        path.push_back(cur);
        if (cur == result.source) break;
        if (cur >= result.parent.size()) {
            path.clear();
            return path;
        }
    }

    std::reverse(path.begin(), path.end());
    return path;
}
