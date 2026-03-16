#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
using namespace std;

class VirtualMemoryCalculator {
private:
    long long addressSpaceBits;
    long long pageSize;
    long long physicalMemorySize;
    long long pageTableEntrySize;

    long long power(int base, int exp) {
        long long result = 1;
        for (int i = 0; i < exp; i++) {
            result *= base;
        }
        return result;
    }

    int log2(long long n) {
        int count = 0;
        while (n > 1) {
            n /= 2;
            count++;
        }
        return count;
    }

    string formatBytes(long long bytes) {
        if (bytes >= 1099511627776LL) { // 1 TB
            return to_string(bytes / 1099511627776LL) + " TB";
        } else if (bytes >= 1073741824) { // 1 GB
            return to_string(bytes / 1073741824) + " GB";
        } else if (bytes >= 1048576) { // 1 MB
            return to_string(bytes / 1048576) + " MB";
        } else if (bytes >= 1024) { // 1 KB
            return to_string(bytes / 1024) + " KB";
        }
        return to_string(bytes) + " bytes";
    }

public:
    void calculateBasicPaging() {
        cout << "\n========== BASIC PAGING CALCULATOR ==========" << endl;

        cout << "Enter virtual address space size in bits (e.g., 32, 64): ";
        cin >> addressSpaceBits;

        cout << "Enter page size in bytes (e.g., 4096, 8192): ";
        cin >> pageSize;

        // Calculate offset bits
        int offsetBits = log2(pageSize);

        // Calculate page number bits
        int pageNumberBits = addressSpaceBits - offsetBits;

        // Calculate number of pages
        long long numPages = power(2, pageNumberBits);

        // Calculate virtual address space size
        long long addressSpaceSize = power(2, addressSpaceBits);

        cout << "\n=== RESULTS ===" << endl;
        cout << "Virtual Address Space Size: " << formatBytes(addressSpaceSize) << endl;
        cout << "Page Size: " << formatBytes(pageSize) << endl;
        cout << "Number of Pages: " << numPages << endl;
        cout << "Offset Bits: " << offsetBits << endl;
        cout << "Page Number Bits: " << pageNumberBits << endl;

        // Page table size calculation
        cout << "\nEnter page table entry size in bytes (typically 4 or 8): ";
        cin >> pageTableEntrySize;

        long long pageTableSize = numPages * pageTableEntrySize;
        cout << "Page Table Size: " << formatBytes(pageTableSize) << endl;

        // Show address format
        cout << "\n=== ADDRESS FORMAT ===" << endl;
        cout << "Bit Layout:" << endl;
        cout << "  Bits " << addressSpaceBits - 1 << " - " << offsetBits
             << ": Page Number (" << pageNumberBits << " bits)" << endl;
        cout << "  Bits " << offsetBits - 1 << " - 0: Offset ("
             << offsetBits << " bits)" << endl;
    }

    void calculateTwoLevelPaging() {
        cout << "\n========== TWO-LEVEL PAGING CALCULATOR ==========" << endl;

        cout << "Enter virtual address space size in bits: ";
        cin >> addressSpaceBits;

        cout << "Enter page size in bytes: ";
        cin >> pageSize;

        cout << "Enter page table entry size in bytes: ";
        cin >> pageTableEntrySize;

        // Calculate offset bits
        int offsetBits = log2(pageSize);

        // Calculate how many PTEs fit in one page
        long long ptesPerPage = pageSize / pageTableEntrySize;
        int secondLevelBits = log2(ptesPerPage);

        // Calculate first level bits
        int firstLevelBits = addressSpaceBits - offsetBits - secondLevelBits;

        long long numFirstLevelEntries = power(2, firstLevelBits);
        long long numSecondLevelTables = numFirstLevelEntries;

        cout << "\n=== TWO-LEVEL PAGE TABLE STRUCTURE ===" << endl;
        cout << "Offset Bits: " << offsetBits << endl;
        cout << "Second-Level Index Bits: " << secondLevelBits << endl;
        cout << "First-Level Index Bits: " << firstLevelBits << endl;
        cout << "Entries per page: " << ptesPerPage << endl;
        cout << "Number of First-Level Entries: " << numFirstLevelEntries << endl;

        long long outerPageTableSize = numFirstLevelEntries * pageTableEntrySize;
        cout << "\nOuter Page Table Size: " << formatBytes(outerPageTableSize) << endl;

        long long maxInnerTablesSize = numSecondLevelTables * pageSize;
        cout << "Max Inner Page Tables Size: " << formatBytes(maxInnerTablesSize) << endl;

        cout << "\n=== ADDRESS FORMAT ===" << endl;
        cout << "Bit Layout:" << endl;
        cout << "  Bits " << addressSpaceBits - 1 << " - "
             << (offsetBits + secondLevelBits)
             << ": Outer Index (" << firstLevelBits << " bits)" << endl;
        cout << "  Bits " << (offsetBits + secondLevelBits - 1) << " - "
             << offsetBits << ": Inner Index (" << secondLevelBits << " bits)" << endl;
        cout << "  Bits " << offsetBits - 1 << " - 0: Offset ("
             << offsetBits << " bits)" << endl;
    }

    void calculatePhysicalMemory() {
        cout << "\n========== PHYSICAL MEMORY CALCULATOR ==========" << endl;

        cout << "Enter physical address size in bits: ";
        int physicalAddressBits;
        cin >> physicalAddressBits;

        cout << "Enter page/frame size in bytes: ";
        cin >> pageSize;

        int offsetBits = log2(pageSize);
        int frameNumberBits = physicalAddressBits - offsetBits;

        long long numFrames = power(2, frameNumberBits);
        long long physicalMemory = power(2, physicalAddressBits);

        cout << "\n=== RESULTS ===" << endl;
        cout << "Physical Memory Size: " << formatBytes(physicalMemory) << endl;
        cout << "Frame Size: " << formatBytes(pageSize) << endl;
        cout << "Number of Frames: " << numFrames << endl;
        cout << "Frame Number Bits: " << frameNumberBits << endl;
        cout << "Offset Bits: " << offsetBits << endl;
    }

    void calculateEffectiveAccessTime() {
        cout << "\n========== EFFECTIVE ACCESS TIME CALCULATOR ==========" << endl;

        double memoryAccessTime, tlbAccessTime, hitRatio;

        cout << "Enter memory access time (ns): ";
        cin >> memoryAccessTime;

        cout << "Enter TLB access time (ns): ";
        cin >> tlbAccessTime;

        cout << "Enter TLB hit ratio (0.0 - 1.0): ";
        cin >> hitRatio;

        // On TLB hit: TLB access + memory access
        // On TLB miss: TLB access + page table access + memory access
        double eatWithTLB = (hitRatio * (tlbAccessTime + memoryAccessTime)) +
                            ((1 - hitRatio) * (tlbAccessTime + 2 * memoryAccessTime));

        double slowdown = eatWithTLB / memoryAccessTime;

        cout << "\n=== RESULTS ===" << endl;
        cout << fixed << setprecision(2);
        cout << "Effective Access Time: " << eatWithTLB << " ns" << endl;
        cout << "Slowdown Factor: " << slowdown << "x" << endl;
        cout << "Performance overhead: " << (slowdown - 1) * 100 << "%" << endl;

        // Show what hit ratio is needed for different performance targets
        cout << "\n=== HIT RATIO ANALYSIS ===" << endl;
        double targetOverheads[] = {0.05, 0.10, 0.20};
        for (double overhead : targetOverheads) {
            double requiredHitRatio = (memoryAccessTime * (1 + overhead) - tlbAccessTime - 2 * memoryAccessTime) /
                                     (memoryAccessTime - tlbAccessTime);
            if (requiredHitRatio >= 0 && requiredHitRatio <= 1) {
                cout << "For " << (overhead * 100) << "% overhead, need hit ratio: "
                     << (requiredHitRatio * 100) << "%" << endl;
            }
        }
    }

    void addressExample() {
        cout << "\n========== ADDRESS TRANSLATION EXAMPLE ==========" << endl;

        cout << "Enter virtual address space bits: ";
        cin >> addressSpaceBits;

        cout << "Enter page size in bytes: ";
        cin >> pageSize;

        cout << "Enter a logical address (decimal): ";
        long long logicalAddress;
        cin >> logicalAddress;

        int offsetBits = log2(pageSize);
        long long offsetMask = pageSize - 1;

        long long pageNumber = logicalAddress / pageSize;
        long long offset = logicalAddress & offsetMask;

        cout << "\n=== ADDRESS BREAKDOWN ===" << endl;
        cout << "Logical Address: " << logicalAddress << " (decimal)" << endl;
        cout << "Page Number: " << pageNumber << endl;
        cout << "Offset: " << offset << endl;

        cout << "\nAssuming page " << pageNumber << " maps to frame: ";
        long long frameNumber;
        cin >> frameNumber;

        long long physicalAddress = (frameNumber * pageSize) + offset;
        cout << "Physical Address: " << physicalAddress << " (decimal)" << endl;
    }

    void mainMenu() {
        int choice;
        do {
            cout << "\n\n========================================" << endl;
            cout << "VIRTUAL MEMORY CALCULATOR" << endl;
            cout << "========================================" << endl;
            cout << "1. Basic Paging Calculations" << endl;
            cout << "2. Two-Level Paging Calculations" << endl;
            cout << "3. Physical Memory Calculations" << endl;
            cout << "4. Effective Access Time with TLB" << endl;
            cout << "5. Address Translation Example" << endl;
            cout << "0. Exit" << endl;
            cout << "========================================" << endl;
            cout << "Enter choice: ";
            cin >> choice;

            switch(choice) {
                case 1:
                    calculateBasicPaging();
                    break;
                case 2:
                    calculateTwoLevelPaging();
                    break;
                case 3:
                    calculatePhysicalMemory();
                    break;
                case 4:
                    calculateEffectiveAccessTime();
                    break;
                case 5:
                    addressExample();
                    break;
                case 0:
                    cout << "Exiting..." << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        } while (choice != 0);
    }
};

int main() {
    VirtualMemoryCalculator calc;
    calc.mainMenu();
    return 0;
}
