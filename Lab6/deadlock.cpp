#include <iostream>
#include <thread>
#include <mutex>

std::mutex m1;
std::mutex m2;

void thread_A_func() {
    std::lock_guard<std::mutex> lock_a(m1); // Locks m1
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Gives thread B time to lock m2
    std::lock_guard<std::mutex> lock_b(m2); // Tries to lock m2, waits indefinitely if thread B has it
    std::cout << "Thread A locked both mutexes" << std::endl;
}

void thread_B_func() {
    std::lock_guard<std::mutex> lock_b(m2); // Locks m2
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Gives thread A time to lock m1
    std::lock_guard<std::mutex> lock_a(m1); // Tries to lock m1, waits indefinitely if thread A has it
    std::cout << "Thread B locked both mutexes" << std::endl;
}
// ... main function creates and joins threads ...
