#include <iostream>
#include <vector>
#include <climits>

using namespace std;

class Frame {
public:
    int pageNumber;
    int lastAccess;

    Frame() {
        pageNumber = -1;
        lastAccess = 0;
    }
};

bool pageHit(vector<Frame>& frames, int page, int time) {
    for (auto &f : frames) {
        if (f.pageNumber == page) {
            f.lastAccess = time;
            return true;
        }
    }
    return false;
}

int findLRU(vector<Frame>& frames) {
    int minTime = INT_MAX;
    int index = 0;

    for (int i = 0; i < frames.size(); i++) {
        if (frames[i].lastAccess < minTime) {
            minTime = frames[i].lastAccess;
            index = i;
        }
    }

    return index;
}

int main() {

    vector<int> reference = {7,0,1,2,0,3,0,4,2,3,0,3,2};
    int numFrames = 3;

    vector<Frame> frames(numFrames);

    int time = 0;
    int pageFaults = 0;

    cout << "LRU Page Replacement Simulation\n\n";

    for (int page : reference) {

        time++;

        if (!pageHit(frames, page, time)) {

            pageFaults++;

            int index = findLRU(frames);

            frames[index].pageNumber = page;
            frames[index].lastAccess = time;
        }

        cout << "Page " << page << " | Frames: ";

        for (auto f : frames) {
            if (f.pageNumber == -1)
                cout << "- ";
            else
                cout << f.pageNumber << " ";
        }

        cout << endl;
    }

    cout << "\nTotal LRU Page Faults: " << pageFaults << endl;

    return 0;
}