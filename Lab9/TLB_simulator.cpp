#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

using namespace std;

class TLB {
private:
    int capacity;
    list<int> lru;
    unordered_map<int, list<int>::iterator> table;

public:
    int hits = 0;
    int misses = 0;

    TLB(int size) {
        capacity = size;
    }

    void access(int page) {

        if (table.find(page) != table.end()) {

            hits++;

            lru.erase(table[page]);
            lru.push_front(page);
            table[page] = lru.begin();

            cout << "Page " << page << " -> TLB Hit\n";
        }
        else {

            misses++;

            if (lru.size() == capacity) {
                int last = lru.back();
                lru.pop_back();
                table.erase(last);
            }

            lru.push_front(page);
            table[page] = lru.begin();

            cout << "Page " << page << " -> TLB Miss\n";
        }
    }
};

int main() {

    vector<int> reference = {7,0,1,2,0,3,0,4,2,3,0,3,2};

    int tlbSize = 4;

    TLB tlb(tlbSize);

    for (int page : reference) {
        tlb.access(page);
    }

    int total = tlb.hits + tlb.misses;

    double hitRatio = (double)tlb.hits / total;
    double missRatio = (double)tlb.misses / total;

    int tlbTime = 20;
    int memoryTime = 100;

    double EAT = (hitRatio * (tlbTime + memoryTime)) +
                 (missRatio * (tlbTime + 2 * memoryTime));

    cout << "\nStatistics\n";
    cout << "----------\n";
    cout << "TLB Hits: " << tlb.hits << endl;
    cout << "TLB Misses: " << tlb.misses << endl;
    cout << "Hit Ratio: " << hitRatio << endl;
    cout << "Effective Access Time: " << EAT << " ns\n";

    return 0;
}