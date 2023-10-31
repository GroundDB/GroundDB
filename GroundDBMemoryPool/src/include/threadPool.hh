#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
namespace mempool
{

    class ThreadPool
    {
    public:
        // Initialize threads polling from the thread_queue_id'th queue
        void SetBackgroundThreads(const int num_threads = std::thread::hardware_concurrency());
        // Add jobs to the thread_queue_id'th queue 
        void Schedule(const std::function<void()> &job, const int thread_queue_id);
    private:
        class TaskQueue
        {
            public:
            std::mutex taskQueueMutex;
            std::condition_variable taskQueueCondition;
            std::queue<std::function<void()>> taskQueue;
        };
        void BGThread(const int thread_queue_id);
        std::vector<std::thread> threads;
        std::vector<TaskQueue> taskQueues;
    };
} // namespace mempool