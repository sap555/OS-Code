#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

struct SegmentTableEntry {
    int base;
    int limit;
    bool valid;
    string segmentName;

    SegmentTableEntry(int b = 0, int l = 0, bool v = false, string name = "")
        : base(b), limit(l), valid(v), segmentName(name) {}
};

class SegmentationSystem {
private:
    vector<SegmentTableEntry> segmentTable;
    const int MAX_SEGMENTS = 8;
    int nextAvailableAddress;

public:
    SegmentationSystem() {
        segmentTable.resize(MAX_SEGMENTS);
        nextAvailableAddress = 0;
    }

    bool addSegment(int segmentNumber, int base, int limit, string name = "") {
        if (segmentNumber < 0 || segmentNumber >= MAX_SEGMENTS) {
            cout << "Error: Invalid segment number " << segmentNumber << endl;
            return false;
        }

        if (segmentTable[segmentNumber].valid) {
            cout << "Warning: Segment " << segmentNumber
                 << " already exists. Overwriting..." << endl;
        }

        segmentTable[segmentNumber].base = base;
        segmentTable[segmentNumber].limit = limit;
        segmentTable[segmentNumber].valid = true;
        segmentTable[segmentNumber].segmentName = name;

        cout << "Segment " << segmentNumber << " (" << name << ") added:" << endl;
        cout << "  Base: " << base << endl;
        cout << "  Limit: " << limit << endl;

        return true;
    }

    bool addSegmentAuto(int segmentNumber, int size, string name = "") {
        if (segmentNumber < 0 || segmentNumber >= MAX_SEGMENTS) {
            cout << "Error: Invalid segment number " << segmentNumber << endl;
            return false;
        }

        segmentTable[segmentNumber].base = nextAvailableAddress;
        segmentTable[segmentNumber].limit = size;
        segmentTable[segmentNumber].valid = true;
        segmentTable[segmentNumber].segmentName = name;

        nextAvailableAddress += size;

        cout << "Segment " << segmentNumber << " (" << name << ") created:" << endl;
        cout << "  Base: " << segmentTable[segmentNumber].base << endl;
        cout << "  Limit: " << size << endl;

        return true;
    }

    int translateAddress(int segmentNumber, int offset) {
        cout << "\n--- Address Translation ---" << endl;
        cout << "Logical Address: (Segment " << segmentNumber
             << ", Offset " << offset << ")" << endl;

        // Check if segment number is valid
        if (segmentNumber < 0 || segmentNumber >= MAX_SEGMENTS) {
            segmentationFault(segmentNumber, offset, "Invalid segment number");
            return -1;
        }

        // Check if segment exists
        if (!segmentTable[segmentNumber].valid) {
            segmentationFault(segmentNumber, offset, "Segment not in memory");
            return -1;
        }

        // Check if offset is within limit
        if (offset < 0 || offset >= segmentTable[segmentNumber].limit) {
            segmentationFault(segmentNumber, offset, "Offset exceeds segment limit");
            return -1;
        }

        // Calculate physical address
        int physicalAddress = segmentTable[segmentNumber].base + offset;

        cout << "Segment: " << segmentTable[segmentNumber].segmentName << endl;
        cout << "Base Address: " << segmentTable[segmentNumber].base << endl;
        cout << "Limit: " << segmentTable[segmentNumber].limit << endl;
        cout << "Physical Address: " << physicalAddress << endl;
        cout << "Translation successful!" << endl;

        return physicalAddress;
    }

    void displaySegmentTable() {
        cout << "\n=== SEGMENT TABLE ===" << endl;
        cout << setw(8) << "Seg#" << setw(15) << "Name"
             << setw(10) << "Base" << setw(10) << "Limit"
             << setw(10) << "Valid" << endl;
        cout << string(53, '-') << endl;

        for (int i = 0; i < MAX_SEGMENTS; i++) {
            if (segmentTable[i].valid) {
                cout << setw(8) << i
                     << setw(15) << segmentTable[i].segmentName
                     << setw(10) << segmentTable[i].base
                     << setw(10) << segmentTable[i].limit
                     << setw(10) << "Yes" << endl;
            }
        }
    }

    void segmentationFault(int segment, int offset, string reason) {
        cout << "\n*** SEGMENTATION FAULT ***" << endl;
        cout << "Reason: " << reason << endl;
        cout << "Attempted Access:" << endl;
        cout << "  Segment Number: " << segment << endl;
        cout << "  Offset: " << offset << endl;

        if (segment >= 0 && segment < MAX_SEGMENTS &&
            segmentTable[segment].valid) {
            cout << "Segment Details:" << endl;
            cout << "  Name: " << segmentTable[segment].segmentName << endl;
            cout << "  Base: " << segmentTable[segment].base << endl;
            cout << "  Limit: " << segmentTable[segment].limit << endl;
            cout << "  Valid Range: 0 - "
                 << (segmentTable[segment].limit - 1) << endl;
        }
    }

    void displayMemoryLayout() {
        cout << "\n=== MEMORY LAYOUT ===" << endl;

        vector<pair<int, string>> layout;
        for (int i = 0; i < MAX_SEGMENTS; i++) {
            if (segmentTable[i].valid) {
                layout.push_back({segmentTable[i].base,
                    "Segment " + to_string(i) + " (" +
                    segmentTable[i].segmentName + ")"});
            }
        }

        // Sort by base address
        sort(layout.begin(), layout.end());

        for (const auto& item : layout) {
            cout << "Address " << setw(6) << item.first
                 << ": " << item.second << endl;
        }
    }
};

int main() {
    cout << "SEGMENTATION SYSTEM SIMULATOR" << endl;
    cout << "=============================" << endl;

    SegmentationSystem segSys;

    // Create segments for a typical program
    cout << "\n--- Creating Program Segments ---" << endl;
    segSys.addSegmentAuto(0, 1000, "Code");
    segSys.addSegmentAuto(1, 400, "Global Data");
    segSys.addSegmentAuto(2, 300, "Heap");
    segSys.addSegmentAuto(3, 200, "Stack");
    segSys.addSegmentAuto(4, 500, "Shared Library");

    // Display segment table
    segSys.displaySegmentTable();
    segSys.displayMemoryLayout();

    // Test address translations
    cout << "\n\n--- Testing Address Translations ---" << endl;

    // Valid accesses
    segSys.translateAddress(0, 500);   // Code segment
    segSys.translateAddress(1, 200);   // Global data
    segSys.translateAddress(2, 150);   // Heap

    // Invalid accesses
    segSys.translateAddress(0, 1500);  // Exceeds code segment limit
    segSys.translateAddress(5, 100);   // Invalid segment
    segSys.translateAddress(1, -50);   // Negative offset

    // Interactive mode
    char choice;
    cout << "\n\nTest custom addresses? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        int seg, offset;
        do {
            cout << "\nEnter segment number (-1 to quit): ";
            cin >> seg;
            if (seg >= 0) {
                cout << "Enter offset: ";
                cin >> offset;
                segSys.translateAddress(seg, offset);
            }
        } while (seg >= 0);
    }

    return 0;
}
