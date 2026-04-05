#include "graph.h"
#include <algorithm>

TeleportGraph::TeleportGraph() {}

int TeleportGraph::AddNode(float x, float y) {
    TerminalNode newNode = {
        nextId++,
        x, y,
        40.0f, 40.0f, // Box size
        GREEN
    };
    nodes.push_back(newNode);
    
    return newNode.id;
}

void TeleportGraph::AddEdge(int id1, int id2) {
    // Deprecated for RNG 50/50 single block portals
}

int TeleportGraph::GetConnectedNode(int id) const {
    // Deprecated for RNG portals
    return -1;
}

TerminalNode* TeleportGraph::GetNode(int id) {
    for (auto& node : nodes) {
        if (node.id == id) return &node;
    }
    return nullptr;
}

void TeleportGraph::Update(float scrollSpeed) {
    for (auto& node : nodes) {
        node.posX -= scrollSpeed;
    }
}

void TeleportGraph::Draw() const {
    // Draw edges removed for single nodes
    
    // Draw nodes
    for (const auto& node : nodes) {
        DrawRectangleV({node.posX, node.posY}, {node.width, node.height}, node.color);
        DrawRectangleLines(node.posX, node.posY, node.width, node.height, DARKGREEN);
    }
}

bool TeleportGraph::CleanUp(float screenLeftEdge) {
    bool missed = false;
    auto it = nodes.begin();
    while (it != nodes.end()) {
        if (it->posX + it->width < screenLeftEdge) {
            if (it->posX > -900.0f) {
                missed = true;
            }
            it = nodes.erase(it);
        } else {
            ++it;
        }
    }
    return missed;
}

void TeleportGraph::Reset() {
    nodes.clear();
    adjacencyMatrix.clear();
    nextId = 0;
}

void TeleportGraph::SpawnPair(float startX, float heightA, float heightB) {
    // Modified to spawn just one node instead of a pair
    AddNode(startX, heightA);
}
