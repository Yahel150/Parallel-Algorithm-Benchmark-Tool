#pragma once

#include <cstdint>
#include <vector>

using NodeId = uint32_t;
template <typename WeightT = uint32_t>
class Graph;


template <typename WeightT = uint32_t>
struct Edge {
    NodeId u;
    NodeId v;
    WeightT weight;
    uint64_t id;   // useful for deterministic tie-breaking

    Edge() : u(0), v(0), weight(), id(0) {}

    Edge(NodeId u, NodeId v, WeightT w, uint64_t id = 0)
        : u(u), v(v), weight(w), id(id) {}

    bool operator<(const Edge& other) const {
        if (weight != other.weight) return weight < other.weight;
        return id < other.id;
    }
};

template <typename WeightT>
class Graph {
public:
    using WeightType = WeightT;
    using AdjEdge = std::pair<NodeId, WeightT>;

    Graph();
    explicit Graph(NodeId num_vertices, bool directed = true);
    Graph(NodeId num_vertices, const std::vector<Edge<WeightT>>& edges, bool directed = true);

    NodeId add_vertex();

    void add_edge(NodeId u, NodeId v, WeightT weight);

    NodeId num_vertices() const;
    uint64_t num_edges() const;
    bool isDirected() const;

    const std::vector<Edge<WeightT>>& get_edges() const;
    const std::vector<std::vector<AdjEdge>>& get_adjacency() const;

    const std::vector<AdjEdge>& neighbors(NodeId u) const;

    void reserve_edges(uint64_t m);
    void reserve_vertices(NodeId n);

    Graph reverse_graph() const;
private:
    NodeId n;
    bool directed;
    uint64_t next_edge_id;

    std::vector<Edge<WeightT>> edges;
    std::vector<std::vector<AdjEdge>> adj;
};


// ---- Constructors ----

template <typename WeightT>
Graph<WeightT>::Graph() : n(0), directed(true), next_edge_id(0) {}

template <typename WeightT>
Graph<WeightT>::Graph(NodeId num_vertices, bool directed_): n(num_vertices), directed(directed_), next_edge_id(0), adj(num_vertices) {}


template <typename WeightT>
Graph<WeightT>::Graph(NodeId num_vertices,const std::vector<Edge<WeightT>>& edges,bool directed):
n(num_vertices), directed(directed), next_edge_id(0),edges(edges), adj(num_vertices) {
    for (const auto& e : edges) {
        adj[e.u].emplace_back(e.v, e.weight);
        if (!directed) {
            adj[e.v].emplace_back(e.u, e.weight);
        }
        if (e.id >= next_edge_id) {
            next_edge_id = e.id + 1;
        }
    }
}

// ---- Vertex / Edge ----

template <typename WeightT>
NodeId Graph<WeightT>::add_vertex() {
    adj.emplace_back();
    return n++;
}

template <typename WeightT>
void Graph<WeightT>::add_edge(NodeId u, NodeId v, WeightT weight) {
    edges.emplace_back(u, v, weight, next_edge_id++);
    adj[u].emplace_back(v, weight);

    if (!directed) {
        adj[v].emplace_back(u, weight);
    }
}


// ---- Getters ----

template <typename WeightT>
NodeId Graph<WeightT>::num_vertices() const {
    return n;
}

template <typename WeightT>
uint64_t Graph<WeightT>::num_edges() const {
    return edges.size();
}

template <typename WeightT>
bool Graph<WeightT>::isDirected() const {
    return directed;
}

template <typename WeightT>
const std::vector<Edge<WeightT>>& Graph<WeightT>::get_edges() const {
    return edges;
}

template <typename WeightT>
const std::vector<std::vector<typename Graph<WeightT>::AdjEdge>>&
Graph<WeightT>::get_adjacency() const {
    return adj;
}

template <typename WeightT>
const std::vector<typename Graph<WeightT>::AdjEdge>&
Graph<WeightT>::neighbors(NodeId u) const {
    return adj[u];
}

// ---- Reserve ----

template <typename WeightT>
void Graph<WeightT>::reserve_edges(uint64_t m) {
    edges.reserve(m);
}

template <typename WeightT>
void Graph<WeightT>::reserve_vertices(NodeId n) {
    adj.reserve(n);
}

// ---- Reverse graph ----

template <typename WeightT>
Graph<WeightT> Graph<WeightT>::reverse_graph() const {
    Graph<WeightT> rev(n, directed);

    for (const auto& e : edges) {
        rev.edges.emplace_back(e.v, e.u, e.weight, e.id);
        rev.adj[e.v].emplace_back(e.u, e.weight);
    }

    return rev;
}