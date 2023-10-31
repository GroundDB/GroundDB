#include <threadPool.hh>
namespace mempool
{
    void ThreadPool::SetBackgroundThreads(const int num_threads)
    {
        std::vector<TaskQueue> list(num_threads);
        taskQueues.swap(list);
        for (uint32_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(std::thread(&ThreadPool::BGThread, this, i));
        }
    }

    // This thread will never terminate
    void ThreadPool::BGThread(const int thread_queue_id)
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(taskQueues[thread_queue_id].taskQueueMutex);
                taskQueues[thread_queue_id].taskQueueCondition.wait(lock, [this, thread_queue_id]
                                     { return !taskQueues[thread_queue_id].taskQueue.empty();});
                job = taskQueues[thread_queue_id].taskQueue.front();
                taskQueues[thread_queue_id].taskQueue.pop();
            }
            job();
        }
    }

    // To use: thread_pool->Schedule([] { /* ... */ }, thread_queue_id);
    void ThreadPool::Schedule(const std::function<void()> &job, const int thread_queue_id)
    {
        {
            std::unique_lock<std::mutex> lock(taskQueues[thread_queue_id].taskQueueMutex);
            taskQueues[thread_queue_id].taskQueue.push(job);
        }
        taskQueues[thread_queue_id].taskQueueCondition.notify_one();
    }
}