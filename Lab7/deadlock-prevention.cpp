#include <iostream>
#include <thread>
#include <mutex>
using namespace std;
std::mutex resourceA, resourceB, resourceC;

void process1() {
    lock(resourceA, resourceB);
    resourceA.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    resourceB.lock();
    // Work...
    resourceB.unlock();
    resourceA.unlock();
}

void process2() {
    lock(resourceB, resourceC);
    resourceB.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    resourceC.lock();
    // Work...
    resourceC.unlock();
    resourceB.unlock();
}

void process3() {
    lock(resourceC, resourceA);
    resourceC.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    resourceA.lock();
    // Work...
    resourceA.unlock();
    resourceC.unlock();
}
int main() {
    std::thread t1(process1);
    std::thread t2(process2);
    std::thread t3(process3);
    
    t1.join();
    t2.join();
    t3.join();

    std::cout << "Program completed successfully!\n";
    
    return 0;
}