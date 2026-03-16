#include <iostream>
#include <map>
#include <list>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

const int MIN_BLOCK_SIZE = 1;    // 1 KB
const int MAX_BLOCK_SIZE = 1024; // 1024 KB

struct Block {
    int address;
    int size;
    bool isFree;
    int allocID;  // For tracking allocations

    Block(int addr, int s, bool free = true, int id = -1)
        : address(addr), size(s), isFree(free), allocID(id) {}
};

class BuddySystem {
private:
    map<int, list<Block*>> freeLists; // Key: block size
    list<Block*> allBlocks;  // Track all blocks for cleanup
    int nextAllocID;

    int nextPowerOf2(int size) {
        if (size <= 0) return MIN_BLOCK_SIZE;

        int power = 1;
        while (power < size) {
            power *= 2;
        }
        return min(power, MAX_BLOCK_SIZE);
    }

    int findBuddy(int address, int size) {
        // Buddy address calculation: XOR with size
        return address ^ size;
    }

    bool isPowerOf2(int n) {
        return (n > 0) && ((n & (n - 1)) == 0);
    }

    void splitBlock(int size) {
        // Find next larger block
        int largerSize = size * 2;

        while (largerSize <= MAX_BLOCK_SIZE) {
            if (!freeLists[largerSize].empty()) {
                // Found a block to split
                Block* block = freeLists[largerSize].front();
                freeLists[largerSize].pop_front();

                cout << "Splitting block of size " << largerSize
                     << " at address " << block->address << endl;

                // Create two buddy blocks
                int halfSize = largerSize / 2;
                Block* buddy1 = new Block(block->address, halfSize, true);
                Block* buddy2 = new Block(block->address + halfSize, halfSize, true);

                freeLists[halfSize].push_back(buddy1);
                freeLists[halfSize].push_back(buddy2);

                allBlocks.push_back(buddy1);
                allBlocks.push_back(buddy2);

                cout << "  Created block at " << buddy1->address
                     << " (size " << halfSize << ")" << endl;
                cout << "  Created block at " << buddy2->address
                     << " (size " << halfSize << ")" << endl;

                // Delete original block
                allBlocks.remove(block);
                delete block;

                // Continue splitting if needed
                if (halfSize > size) {
                    splitBlock(size);
                }
                return;
            }
            largerSize *= 2;
        }
    }

public:
    BuddySystem() : nextAllocID(1) {
        // Initialize with one large block
        Block* initialBlock = new Block(0, MAX_BLOCK_SIZE, true);
        freeLists[MAX_BLOCK_SIZE].push_back(initialBlock);
        allBlocks.push_back(initialBlock);

        cout << "Buddy System initialized with " << MAX_BLOCK_SIZE
             << " KB memory" << endl;
    }

    ~BuddySystem() {
        // Clean up all blocks
        for (Block* block : allBlocks) {
            delete block;
        }
    }

    Block* allocate(int requestSize) {
        cout << "\n--- Allocation Request ---" << endl;
        cout << "Requested size: " << requestSize << " KB" << endl;

        // Round up to power of 2
        int size = nextPowerOf2(requestSize);
        cout << "Rounded to: " << size << " KB (next power of 2)" << endl;

        if (size > MAX_BLOCK_SIZE) {
            cout << "Error: Request exceeds maximum block size" << endl;
            return nullptr;
        }

        // Check if we have a block of this size
        if (freeLists[size].empty()) {
            cout << "No free block of size " << size << " available" << endl;
            cout << "Attempting to split larger block..." << endl;
            splitBlock(size);
        }

        // Try to allocate again
        if (freeLists[size].empty()) {
            cout << "Allocation FAILED: No memory available" << endl;
            return nullptr;
        }

        // Allocate the block
        Block* block = freeLists[size].front();
        freeLists[size].pop_front();
        block->isFree = false;
        block->allocID = nextAllocID++;

        cout << "Allocated block #" << block->allocID
             << " at address " << block->address
             << " (size " << block->size << " KB)" << endl;
        cout << "Internal fragmentation: " << (size - requestSize)
             << " KB" << endl;

        return block;
    }

    void deallocate(Block* block) {
        if (!block) {
            cout << "Error: Null block pointer" << endl;
            return;
        }

        cout << "\n--- Deallocation ---" << endl;
        cout << "Freeing block #" << block->allocID
             << " at address " << block->address
             << " (size " << block->size << " KB)" << endl;

        block->isFree = true;
        block->allocID = -1;

        // Try to coalesce with buddy
        coalesce(block);
    }

    void coalesce(Block* block) {
        int buddyAddr = findBuddy(block->address, block->size);

        cout << "Checking for buddy at address " << buddyAddr << endl;

        // Find buddy in free list
        Block* buddy = nullptr;
        for (Block* b : freeLists[block->size]) {
            if (b->address == buddyAddr && b->isFree) {
                buddy = b;
                break;
            }
        }

        if (buddy && block->size * 2 <= MAX_BLOCK_SIZE) {
            cout << "Buddy found! Coalescing blocks..." << endl;

            // Remove both blocks from free list
            freeLists[block->size].remove(block);
            freeLists[block->size].remove(buddy);

            // Create merged block
            int newAddr = min(block->address, buddy->address);
            int newSize = block->size * 2;
            Block* merged = new Block(newAddr, newSize, true);

            cout << "Merged into block at address " << newAddr
                 << " (size " << newSize << " KB)" << endl;

            // Add to appropriate free list
            freeLists[newSize].push_back(merged);
            allBlocks.push_back(merged);

            // Remove old blocks
            allBlocks.remove(block);
            allBlocks.remove(buddy);
            delete block;
            delete buddy;

            // Recursively try to coalesce further
            coalesce(merged);
        } else {
            // No buddy found, just add to free list
            freeLists[block->size].push_back(block);
            cout << "No buddy available for coalescing" << endl;
        }
    }

    void displayFreeLists() {
        cout << "\n=== FREE LISTS ===" << endl;
        cout << setw(12) << "Block Size" << setw(15) << "Free Blocks" << endl;
        cout << string(27, '-') << endl;

        for (int size = MIN_BLOCK_SIZE; size <= MAX_BLOCK_SIZE; size *= 2) {
            cout << setw(12) << (to_string(size) + " KB");

            if (freeLists[size].empty()) {
                cout << setw(15) << "None" << endl;
            } else {
                cout << setw(15) << freeLists[size].size() << " block(s)" << endl;
                for (Block* block : freeLists[size]) {
                    cout << string(12, ' ') << "  @ address "
                         << block->address << endl;
                }
            }
        }
    }

    void displayAllBlocks() {
        cout << "\n=== ALL BLOCKS ===" << endl;
        cout << setw(10) << "Address" << setw(10) << "Size"
             << setw(10) << "Status" << setw(10) << "Alloc ID" << endl;
        cout << string(40, '-') << endl;

        // Create a sorted list of blocks
        list<Block*> sorted = allBlocks;
        sorted.sort([](Block* a, Block* b) { return a->address < b->address; });

        for (Block* block : sorted) {
            cout << setw(10) << block->address
                 << setw(10) << block->size
                 << setw(10) << (block->isFree ? "FREE" : "USED")
                 << setw(10) << (block->isFree ? "-" : to_string(block->allocID))
                 << endl;
        }
    }

    void displayStats() {
        int totalFree = 0;
        int totalUsed = 0;
        int numFreeBlocks = 0;
        int numUsedBlocks = 0;

        for (Block* block : allBlocks) {
            if (block->isFree) {
                totalFree += block->size;
                numFreeBlocks++;
            } else {
                totalUsed += block->size;
                numUsedBlocks++;
            }
        }

        cout << "\n=== STATISTICS ===" << endl;
        cout << "Total Memory: " << MAX_BLOCK_SIZE << " KB" << endl;
        cout << "Used Memory: " << totalUsed << " KB" << endl;
        cout << "Free Memory: " << totalFree << " KB" << endl;
        cout << "Number of Used Blocks: " << numUsedBlocks << endl;
        cout << "Number of Free Blocks: " << numFreeBlocks << endl;

        double utilization = (double)totalUsed / MAX_BLOCK_SIZE * 100;
        cout << "Memory Utilization: " << fixed << setprecision(2)
             << utilization << "%" << endl;
    }
};

int main() {
    cout << "BUDDY SYSTEM MEMORY ALLOCATOR" << endl;
    cout << "==============================" << endl;
    cout << "Memory Size: " << MIN_BLOCK_SIZE << " KB - "
         << MAX_BLOCK_SIZE << " KB" << endl << endl;

    BuddySystem buddy;

    // Test allocations
    cout << "\n========== TEST SCENARIO ==========" << endl;

    Block* b1 = buddy.allocate(100);
    buddy.displayFreeLists();

    Block* b2 = buddy.allocate(240);
    buddy.displayFreeLists();

    Block* b3 = buddy.allocate(64);
    buddy.displayFreeLists();

    Block* b4 = buddy.allocate(256);
    buddy.displayFreeLists();

    buddy.displayAllBlocks();
    buddy.displayStats();

    // Test deallocations and coalescing
    cout << "\n\n========== DEALLOCATION AND COALESCING ==========" << endl;

    buddy.deallocate(b1);
    buddy.displayFreeLists();

    buddy.deallocate(b3);
    buddy.displayFreeLists();

    buddy.deallocate(b2);
    buddy.displayFreeLists();

    buddy.deallocate(b4);
    buddy.displayFreeLists();

    buddy.displayAllBlocks();
    buddy.displayStats();

    return 0;
}
