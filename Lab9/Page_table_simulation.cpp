#include <iostream>
#include <vector>
using namespace std;

class PageTableEntry {
private:
    int frameNumber;
    bool valid;
    bool dirty;
    bool referenced;

public:
    PageTableEntry() {
        frameNumber = -1;
        valid = false;
        dirty = false;
        referenced = false;
    }

    void setEntry(int frame, bool v, bool d, bool r) {
        frameNumber = frame;
        valid = v;
        dirty = d;
        referenced = r;
    }

    int getFrameNumber() { return frameNumber; }
    bool isValid() { return valid; }
    bool isDirty() { return dirty; }
    bool isReferenced() { return referenced; }

    void display(int pageNumber) {
        cout << "Page " << pageNumber
             << " | Frame: " << frameNumber
             << " | Valid: " << valid
             << " | Dirty: " << dirty
             << " | Ref: " << referenced << endl;
    }
};

class PageTable {
private:
    vector<PageTableEntry> table;
    int pageSize;

public:
    PageTable(int numPages, int pSize) {
        pageSize = pSize;
        table.resize(numPages);
    }

    void setPage(int page, int frame, bool valid, bool dirty, bool ref) {
        table[page].setEntry(frame, valid, dirty, ref);
    }

    void translate(int virtualAddress) {
        int pageNumber = virtualAddress / pageSize;
        int offset = virtualAddress % pageSize;

        cout << "Virtual Address: " << virtualAddress << endl;
        cout << "Page Number: " << pageNumber << ", Offset: " << offset << endl;

        if (!table[pageNumber].isValid()) {
            cout << "Page Fault!" << endl;
            return;
        }

        int frame = table[pageNumber].getFrameNumber();
        int physicalAddress = frame * pageSize + offset;

        cout << "Physical Address: " << physicalAddress << endl;
    }

    void displayTable() {
        cout << "\nPage Table State:\n";
        for (int i = 0; i < table.size(); i++) {
            table[i].display(i);
        }
    }
};

int main() {
    int numPages = 4;
    int pageSize = 1024;

    PageTable pt(numPages, pageSize);

    pt.setPage(0, 5, true, false, true);
    pt.setPage(1, 3, true, false, false);
    pt.setPage(2, 5, true, true, true);
    pt.setPage(3, -1, false, false, false);

    pt.displayTable();

    cout << endl;
    pt.translate(1024);

    return 0;
}