#pragma once

#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------
// Indexed binary min-heap with decrease-key in O(log n)
// Eliminates stale entries vs lazy-deletion, giving O(E log V) instead of
// O(E log E).
// ---------------------------------------------------------------------------
template <typename WeightT>
struct IndexedMinHeap {
    struct Entry { WeightT key; NodeId node; };

    std::vector<Entry>   heap;   // heap[pos] = {key, node}
    std::vector<int>     pos;    // pos[node]  = index into heap, -1 = not present
    std::vector<WeightT> keys;   // keys[node] = current best key

    explicit IndexedMinHeap(NodeId n)
        : pos(n, -1), keys(n, std::numeric_limits<WeightT>::max()) {}

    bool empty()    const { return heap.empty(); }
    bool contains(NodeId v) const { return pos[v] >= 0; }

    void push(NodeId v, WeightT k) {
        keys[v] = k;
        pos[v]  = static_cast<int>(heap.size());
        heap.push_back({k, v});
        sift_up(pos[v]);
    }

    // Decrease key of an already-present node.
    void decrease_key(NodeId v, WeightT k) {
        keys[v]        = k;
        heap[pos[v]].key = k;
        sift_up(pos[v]);
    }

    Entry pop_min() {
        Entry top = heap[0];
        pos[top.node] = -1;
        if (heap.size() > 1) {
            heap[0] = heap.back();
            pos[heap[0].node] = 0;
        }
        heap.pop_back();
        if (!heap.empty()) sift_down(0);
        return top;
    }

private:
    void swap_entries(int i, int j) {
        std::swap(heap[i], heap[j]);
        pos[heap[i].node] = i;
        pos[heap[j].node] = j;
    }

    void sift_up(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[p].key <= heap[i].key) break;
            swap_entries(i, p);
            i = p;
        }
    }

    void sift_down(int i) {
        int sz = static_cast<int>(heap.size());
        while (true) {
            int smallest = i, l = 2*i+1, r = 2*i+2;
            if (l < sz && heap[l].key < heap[smallest].key) smallest = l;
            if (r < sz && heap[r].key < heap[smallest].key) smallest = r;
            if (smallest == i) break;
            swap_entries(i, smallest);
            i = smallest;
        }
    }
};