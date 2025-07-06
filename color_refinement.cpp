#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <chrono>
#include <iomanip>

struct Node {
    int id;
    int color;
    std::vector<int> neighbors;

    Node(int an_id) : id(an_id), color(1) {} // All nodes start with the same color (1).
};

class Graph {
public:
    int numNodes;
    std::vector<Node> nodes;

    Graph(int n) : numNodes(n) {
        for (int i = 0; i < n; ++i) {
            nodes.emplace_back(i);
        }
    }

    void readAdjMatrix() {
        for (int i = 0; i < numNodes; ++i) {
            std::string row;
            std::cin >> row;
            for (int j = 0; j < numNodes; ++j) {
                if (row[j] == '1') {
                    nodes[i].neighbors.push_back(j);
                }
            }
        }
    }

    // Generates a histogram of colors, mapping each color to the count of nodes with that color.`
    // This is used for the final comparison.
    std::map<int, int> getHistogram() const {
        std::map<int, int> histogram;
        for (const auto& node : nodes) {
            histogram[node.color]++;
        }
        return histogram;
    }
};

class ColorRefinement {
public:
    bool arePotentiallyIsomorphic(Graph& g1, Graph& g2) {
        // The algorithm iteratively refines colors until they stabilize.
        while (true) {
            bool isStable1 = refineColors(g1);
            bool isStable2 = refineColors(g2);

            if (isStable1 && isStable2) {
                break;
            }

            // An intermediate check: if histograms differ mid-process, they can't be isomorphic.
            if (g1.getHistogram() != g2.getHistogram()) {
                return false;
            }
        }

        // Final check: Compare the color histograms of the stabilized graphs.
        // If the histograms are identical, they pass the color refinement test.
        return g1.getHistogram() == g2.getHistogram();
    }

private:
    // Performs a single iteration of color refinement on a graph.
    // Returns true if any node's color was changed, false otherwise.
    bool refineColors(Graph& g) {
        // The "signature" of a node is its own color combined with the sorted list of its neighbors' colors.
        // We use a map to group nodes that have the exact same signature.
        // Key: A pair containing {node's current color, sorted vector of neighbor colors}.
        // Value: A vector of node indices that share this signature.
        std::map<std::pair<int, std::vector<int>>, std::vector<int>> signatureToNodesMap;

        for (const auto& node : g.nodes) {
            std::vector<int> neighborColors;
            // Collect the current colors of all neighbors.
            for (int neighbor_idx : node.neighbors) {
                neighborColors.push_back(g.nodes[neighbor_idx].color);
            }
            // Sort the neighbor colors to create a canonical representation.
            // The order of neighbors doesn't matter, only the collection of their colors.
            std::sort(neighborColors.begin(), neighborColors.end());

            // Create the signature and group the current node with others that have the same one.
            signatureToNodesMap[{node.color, neighborColors}].push_back(node.id);
        }

        // Now, assign new colors based on the groupings.
        // Each unique signature group gets a new, unique color.
        bool isStable = true;
        int newColor = 1;
        std::vector<int> nextColors(g.numNodes);

        for (const auto& pair : signatureToNodesMap) {
            const auto& node_indices = pair.second;
            for (int idx : node_indices) {
                nextColors[idx] = newColor;
            }
            newColor++;
        }
        
        // Apply the new colors to the graph and check if isStable.
        for(int i = 0; i < g.numNodes; ++i) {
            if(g.nodes[i].color != nextColors[i]) {
                isStable = false;
            }
            g.nodes[i].color = nextColors[i];
        }

        return isStable;
    }
};


int main() {
    int n;
    int instance_count = 1;
    
    while (std::cin >> n) {
        Graph g1(n);
        Graph g2(n);

        g1.readAdjMatrix();
        g2.readAdjMatrix();

        auto start = std::chrono::high_resolution_clock::now();

        ColorRefinement algorithm;
        bool isomorphic = algorithm.arePotentiallyIsomorphic(g1, g2);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> cpu_time = end - start;

        std::cout << instance_count << ") n = " << n << " "
                  << (isomorphic ? "+++" : "---") << " "
                  << std::fixed << std::setprecision(3) << cpu_time.count() << std::endl;
        
        instance_count++;
    }

    return 0;
}
