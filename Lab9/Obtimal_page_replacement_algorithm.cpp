#include <iostream>
#include <vector>
using namespace std;

bool search(int key, vector<int>& frames) {
    for (int f : frames)
        if (f == key)
            return true;
    return false;
}

int predict(vector<int>& pages, vector<int>& frames, int index) {

    int res = -1, farthest = index;

    for (int i = 0; i < frames.size(); i++) {

        int j;

        for (j = index; j < pages.size(); j++) {

            if (frames[i] == pages[j]) {

                if (j > farthest) {
                    farthest = j;
                    res = i;
                }
                break;
            }
        }

        if (j == pages.size())
            return i;
    }

    if (res == -1)
        return 0;
    else
        return res;
}

int optimalPage(vector<int> pages, int framesSize) {

    vector<int> frames;
    int faults = 0;

    for (int i = 0; i < pages.size(); i++) {

        if (search(pages[i], frames))
            continue;

        if (frames.size() < framesSize) {
            frames.push_back(pages[i]);
        }

        else {

            int j = predict(pages, frames, i + 1);
            frames[j] = pages[i];
        }

        faults++;
    }

    return faults;
}

int main() {

    vector<int> pages =
    {7,0,1,2,0,3,0,4,2,3,0,3,2};

    int frames = 3;

    int faults = optimalPage(pages, frames);

    cout << "Optimal Page Faults: "
         << faults << endl;

    return 0;
}