#pragma once
#include <vector>
#include "raylib.h"

struct TerminalNode {
    int id;
    float posX;
    float posY;
    float width;
    float height;
    Color color;
};

class TeleportGraph {
private:
    std::vector<TerminalNode> nodes;
    std::vector<std::vector<bool>> adjacencyMatrix;
    int nextId = 0;

public:
    TeleportGraph();
    
    // Add a new terminal node to the graph and return its ID
    int AddNode(float x, float y);
    
    // Connect two nodes
    void AddEdge(int id1, int id2);

    // Get connection target (assuming pairs for now)
    int GetConnectedNode(int id) const;
    
    // Update positions based on camera scroll
    void Update(float scrollSpeed);
    
    // Render the nodes and physical lines between connected ones
    void Draw() const;
    
    // Get node by ID safely
    TerminalNode* GetNode(int id);
    
    // Clear out off-screen nodes and return true if a node was missed
    bool CleanUp(float screenLeftEdge);

    // Wipe entire graph (for game over reset)
    void Reset();

    // Spawn a functional pair on the map
    void SpawnPair(float startX, float heightA, float heightB);
    
    // Get all nodes for collision detection
    const std::vector<TerminalNode>& GetNodes() const { return nodes; }
};
