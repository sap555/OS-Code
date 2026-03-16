#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

int main() {

    string filename = "data.txt";
    const int PAGE_SIZE = 4096;

    ifstream file(filename, ios::binary | ios::ate);

    if (!file) {
        cout << "Cannot open file\n";
        return 1;
    }

    int fileSize = file.tellg();
    file.seekg(0);

    int numPages = ceil((double)fileSize / PAGE_SIZE);

    vector<bool> pageLoaded(numPages, false);

    cout << "File size: " << fileSize << " bytes\n";
    cout << "Pages: " << numPages << endl;

    while (true) {

        int offset;
        cout << "\nEnter offset to read (-1 to exit): ";
        cin >> offset;

        if (offset == -1) break;

        if (offset >= fileSize) {
            cout << "Invalid offset\n";
            continue;
        }

        int page = offset / PAGE_SIZE;

        if (!pageLoaded[page]) {

            cout << "Page Fault -> Loading Page " << page << endl;
            pageLoaded[page] = true;
        }
        else {

            cout << "Page " << page << " already in memory\n";
        }

        file.seekg(offset);
        char c;
        file.get(c);

        cout << "Character at offset " << offset << ": " << c << endl;
    }

    cout << "\nPages Loaded:\n";

    for (int i = 0; i < numPages; i++) {

        cout << "Page " << i << ": ";

        if (pageLoaded[i])
            cout << "Loaded\n";
        else
            cout << "Not Loaded\n";
    }

    file.close();

    return 0;
}