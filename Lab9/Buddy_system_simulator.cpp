#include <iostream>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

class BuddyAllocator {

    int totalSize;
    map<int, vector<int>> freeList;
    map<int, int> allocated;

public:

    BuddyAllocator(int size) {
        totalSize = size;
        int order = log2(size);
        freeList[order].push_back(0);
    }

    int getOrder(int size) {
        int order = 0;
        int block = 1;

        while (block < size) {
            block *= 2;
            order++;
        }

        return order;
    }

    int allocate(int size) {

        int order = getOrder(size);
        int current = order;

        while (freeList[current].empty())
            current++;

        if (current > log2(totalSize)) {
            cout << "Allocation failed\n";
            return -1;
        }

        int addr = freeList[current].back();
        freeList[current].pop_back();

        while (current > order) {

            current--;

            int buddy = addr + (1 << current);

            freeList[current].push_back(buddy);
        }

        allocated[addr] = order;

        cout << "Allocated " << size
             << " KB at address " << addr
             << " (block size " << (1 << order) << ")\n";

        return addr;
    }

    void deallocate(int addr) {

        if (allocated.find(addr) == allocated.end()) {
            cout << "Invalid free\n";
            return;
        }

        int order = allocated[addr];
        allocated.erase(addr);

        freeList[order].push_back(addr);

        cout << "Freed block at address " << addr << endl;
    }

    void display() {

        cout << "\nFree Lists:\n";

        for (auto &p : freeList) {

            cout << "Block size " << (1 << p.first) << ": ";

            for (int addr : p.second)
                cout << addr << " ";

            cout << endl;
        }
    }
};

int main() {

    BuddyAllocator allocator(1024);

    int a = allocator.allocate(70);
    int b = allocator.allocate(35);
    int c = allocator.allocate(80);

    allocator.display();

    allocator.deallocate(a);

    allocator.display();

    return 0;
}