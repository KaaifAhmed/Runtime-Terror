#include <iostream>
#include <vector>

using namespace std;

// 1. A simplified version of what you want to save
struct PlayerState {
    float x;
    float y;
};

// 2. The Data Structure
class RewindBuffer {
private:
    vector<PlayerState> buffer;
    int head;     // Where to write the next state
    int count;    // How many states are currently stored
    int capacity; // Maximum states (e.g., 300)

public:
    RewindBuffer(int cap) {
        capacity = cap;
        buffer.resize(capacity);
        head = 0;
        count = 0;
    }

    // Called every frame during normal gameplay
    void Record(PlayerState state) {
        buffer[head] = state;                // Write data
        head = (head + 1) % capacity;        // Move head forward, wrap around if at end
        
        if (count < capacity) {
            count++;                         // Increase count until full
        }
    }

    // Called every frame when the player holds 'Ctrl+Z'
    bool Rewind(PlayerState& outState) {
        if (count == 0) {
            return false; // Nothing left to rewind
        }

        // Move head backwards, wrapping around properly
        head = (head - 1 + capacity) % capacity; 
        
        outState = buffer[head];             // Retrieve the state
        count--;                             // We "consumed" a state
        
        return true;
    }
    
    int GetCount() { return count; }
};

// 3. The Isolated Test
int main() {
    // Set a tiny capacity of 5 to easily test the overwrite logic
    RewindBuffer rewindSys(5);

    cout << "--- RECORDING PHASE ---\n";
    // Simulate 8 frames of gameplay (This will overflow our capacity of 5)
    while (true)
    {
        cout << "\nRecord Frame: ";
        int choice;
        cin >> choice;

        if (choice == -1)
        {
            break;
        }

        PlayerState currentState = { (float)choice, 0 }; // Moving right by 10 pixels
        rewindSys.Record(currentState);
        cout << "Recorded Frame: X=" << currentState.x << "\n";
        
    }

    cout << "\n--- REWIND PHASE ---\n";
    cout << "States stored: " << rewindSys.GetCount() << " (Should be exactly 5)\n";
    
    PlayerState restoredState;
    // Simulate holding the rewind button
    while (rewindSys.Rewind(restoredState)) {
        cout << "Rewound to: X=" << restoredState.x << "\n";
    }

    cout << "Rewind complete. States left: " << rewindSys.GetCount() << "\n";

    return 0;
}