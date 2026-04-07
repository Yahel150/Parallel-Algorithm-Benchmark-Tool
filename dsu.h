    #pragma once

    #include <cstdint>
    #include <vector>

    using DsuIndex = uint32_t;

    template <typename IndexT = DsuIndex>
    class DSU {
    public:
        DSU();
        explicit DSU(IndexT n);

        void reset(IndexT n);

        IndexT add();

        IndexT find(IndexT x);
        IndexT find(IndexT x) const;//non–path-compressing version.

        bool unite(IndexT a, IndexT b);

        bool same(IndexT a, IndexT b) const;
        IndexT size(IndexT x) const;
        IndexT components() const;

        IndexT count() const;

    private:
        std::vector<IndexT> parent;
        std::vector<IndexT> component_size;
        IndexT component_count;
    };



    // ---- Constructors ----

    template <typename IndexT>
    DSU<IndexT>::DSU() : component_count(0) {}

    template <typename IndexT>
    DSU<IndexT>::DSU(IndexT n) {
        reset(n);
    }

    // ---- Reset ----

    template <typename IndexT>
    void DSU<IndexT>::reset(IndexT n) {
        parent.resize(n);
        component_size.assign(n, 1);
        component_count = n;

        for (IndexT i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }

    // ---- Add ----
    template <typename IndexT>
    IndexT DSU<IndexT>::add() {
        IndexT id = parent.size();
        parent.push_back(id);
        component_size.push_back(1);
        ++component_count;
        return id;
    }

    // ---- Find (with compression) ----

    template <typename IndexT>
    IndexT DSU<IndexT>::find(IndexT x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }


    // ---- Find (const, no compression) ----
    template <typename IndexT>
    IndexT DSU<IndexT>::find(IndexT x) const {
        while (parent[x] != x) {
            x = parent[x];
        }
        return x;
    }


    // ---- Unite ----
    template <typename IndexT>
    bool DSU<IndexT>::unite(IndexT a, IndexT b) {
        a = find(a);
        b = find(b);

        if (a == b) return false;

        if (component_size[a] < component_size[b]) {
            IndexT tmp = a;
            a = b;
            b = tmp;
        }

        parent[b] = a;
        component_size[a] += component_size[b];
        --component_count;

        return true;
    }

    // ---- Queries ----

    template <typename IndexT>
    bool DSU<IndexT>::same(IndexT a, IndexT b) const {
        return find(a) == find(b);
    }

    template <typename IndexT>
    IndexT DSU<IndexT>::size(IndexT x) const {
        x = find(x);
        return component_size[x];
    }

    template <typename IndexT>
    IndexT DSU<IndexT>::components() const {
        return component_count;
    }

    template <typename IndexT>
    IndexT DSU<IndexT>::count() const {
        return parent.size();
    }