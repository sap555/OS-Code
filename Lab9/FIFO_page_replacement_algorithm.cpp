#include <iostream>
#include <vector>
#include <queue>
using namespace std;

int main() {
    vector<int> reference = {7,0,1,2,0,3,0,4,2,3,0,3,2};
    int frames = 3;

    vector<int> memory(frames, -1);
    queue<int> fifo;

    int pageFaults = 0;

    cout << "FIFO Page Replacement Simulation\n\n";

    for(int page : reference) {

        bool hit = false;

        for(int i = 0; i < frames; i++) {
            if(memory[i] == page) {
                hit = true;
                break;
            }
        }

        if(!hit) {
            pageFaults++;

            if(fifo.size() < frames) {
                memory[fifo.size()] = page;
                fifo.push(fifo.size());
            }
            else {
                int index = fifo.front();
                fifo.pop();

                memory[index] = page;
                fifo.push(index);
            }
        }

        cout << "Page: " << page << " | Frames: ";
        for(int f : memory) {
            if(f == -1) cout << "- ";
            else cout << f << " ";
        }

        if(hit) cout << " (Hit)";
        else cout << " (Fault)";

        cout << endl;
    }

    cout << "\nTotal Page Faults: " << pageFaults << endl;

    double faultRate = (double)pageFaults / reference.size();
    cout << "Page Fault Rate: " << faultRate * 100 << "%\n";

    return 0;
}