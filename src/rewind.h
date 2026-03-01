#pragma once

#include <vector>

struct PlayerState
{
    float x;
    float y;
};

class RewindBuffer
{
private:
    std::vector<PlayerState> buffer;
    int head;     // Where to write the next state
    int count;    // How many states are currently stored
    int capacity; // Maximum frames to be stored

public:
    RewindBuffer(int cap)
    {
        capacity = cap;
        buffer.resize(capacity);
        head = 0;
        count = 0;
    }

    // Called every frame during normal gameplay
    void Record(PlayerState state)
    {
        buffer[head] = state;         // Write data
        head = (head + 1) % capacity; // Move head forward, wrap around if at end

        if (count < capacity)
        {
            count++; // Increase count until full
        }
    }

    // Called every frame when the player holds 'Ctrl+Z'
    bool Rewind(PlayerState &outState)
    {
        if (count == 0)
        {
            return false; // Nothing left to rewind
        }

        // Move head backwards, wrapping around properly
        head = (head - 1 + capacity) % capacity;

        outState = buffer[head]; // Retrieve the state
        count--;

        return true;
    }

    int GetCount() { return count; }
};
