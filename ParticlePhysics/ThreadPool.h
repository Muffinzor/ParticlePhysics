#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void wait_all();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<int> active_tasks;

    void worker_loop();
};