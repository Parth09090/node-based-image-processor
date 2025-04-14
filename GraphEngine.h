#pragma once
#include "nodes/Node.h"
#include <unordered_set>
#include <iostream>

class GraphEngine {
public:
    void execute(Node* node, std::unordered_set<Node*>& visited) {
        if (visited.find(node) != visited.end()) {
            return;
        }

        visited.insert(node);

        // ✅ First process all inputs
        for (Node* input : node->inputs) {
            execute(input, visited);
        }

        // ✅ THEN process this node
        node->process();
    }
};
