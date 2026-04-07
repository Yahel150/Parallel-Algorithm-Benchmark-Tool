#pragma once

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include "../dsu.h"
#include "../graph.h"
#include "mst_types.h"

class Barrier {
    std::mutex mtx;
    std::condition_variable cv;
    std::size_t waiting;
    std::size_t total;
    std::size_t generation = 0;

public:
    explicit Barrier(std::size_t n) : waiting(n), total(n) {}

    void arrive_and_wait() {
        std::unique_lock<std::mutex> lock(mtx);
        const std::size_t gen = generation;
        if (--waiting == 0) {
            ++generation;
            waiting = total;
            cv.notify_all();
        } else {
            cv.wait(lock, [&] { return generation != gen; });
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
    if (thread_count == 0) thread_count = 1;
    if (edges.empty()) {
        result.connected = (n == 1);
        return result;
    }

    const std::size_t m = edges.size();
    const std::size_t tcount = std::min(thread_count, m);

    DSU<NodeId> dsu(n);
    std::size_t components = n;
    result.edges.reserve(n - 1);

    std::vector<std::vector<Edge<WeightT>>> local_best(tcount, std::vector<Edge<WeightT>>(n));
    std::vector<std::vector<char>> local_valid(tcount, std::vector<char>(n, 0));
    std::vector<Edge<WeightT>> best(n);
    std::vector<char> valid(n, 0);

    std::atomic<bool> keep_going{true};
    Barrier scan_done(tcount + 1);
    Barrier round_done(tcount + 1);

    std::vector<std::thread> workers;
    workers.reserve(tcount);

    for (std::size_t t = 0; t < tcount; ++t) {
        const std::size_t left = (t * m) / tcount;
        const std::size_t right = ((t + 1) * m) / tcount;

        workers.emplace_back([&, t, left, right] {
            auto& lbest = local_best[t];
            auto& lvalid = local_valid[t];

            while (true) {
                const auto& dsu_ro = static_cast<const DSU<NodeId>&>(dsu);

                for (std::size_t i = left; i < right; ++i) {
                    const auto& e = edges[i];
                    const NodeId ru = dsu_ro.find(e.u);
                    const NodeId rv = dsu_ro.find(e.v);
                    if (ru == rv) continue;

                    mst_detail::update_best_edge(lvalid[ru], lbest[ru], e);
                    mst_detail::update_best_edge(lvalid[rv], lbest[rv], e);
                }

                scan_done.arrive_and_wait();
                round_done.arrive_and_wait();

                if (!keep_going.load(std::memory_order_acquire)) return;
                std::fill(lvalid.begin(), lvalid.end(), 0);
            }
        });
    }

    while (true) {
        scan_done.arrive_and_wait();

        std::fill(valid.begin(), valid.end(), 0);
        for (std::size_t t = 0; t < tcount; ++t) {
            for (NodeId r = 0; r < n; ++r) {
                if (!local_valid[t][r]) continue;
                mst_detail::update_best_edge(valid[r], best[r], local_best[t][r]);
            }
        }

        bool merged_any = false;
        for (NodeId r = 0; r < n && components > 1; ++r) {
            if (!valid[r]) continue;
            const auto& e = best[r];
            if (dsu.unite(e.u, e.v)) {
                result.edges.push_back(e);
                result.total_weight += e.weight;
                --components;
                merged_any = true;
            }
        }

        const bool finished = !merged_any || components <= 1;
        if (finished) keep_going.store(false, std::memory_order_release);

        round_done.arrive_and_wait();
        if (finished) break;
    }

    for (auto& worker : workers) worker.join();

    result.connected = (components == 1);
    return result;
}
