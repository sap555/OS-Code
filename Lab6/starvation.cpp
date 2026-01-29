#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

std::mutex resource_mutex;
std::condition_variable cv;
std::queue<int> request_queue;
int resource_count = 1; // ทรัพยากรจำกัด

void process(int id) {
    std::unique_lock<std::mutex> lock(resource_mutex);
    request_queue.push(id);
    std::cout << "Process " << id << " waiting.\n";

    // รอจนกว่าจะเป็นคิวแรกและมีทรัพยากรว่าง (แก้ปัญหา starvation)
    cv.wait(lock, [id] { return !request_queue.empty() && request_queue.front() == id; });

    // ใช้ทรัพยากร
    request_queue.pop();
    std::cout << "Process " << id << " is using resource.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ปล่อยทรัพยากรและแจ้งเตือนคิวถัดไป
    std::cout << "Process " << id << " finished.\n";
    cv.notify_all(); // แจ้งเตือนทุก Process ให้เช็คคิว
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 1; i <= 5; ++i) {
        threads.push_back(std::thread(process, i));
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
