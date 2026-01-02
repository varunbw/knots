#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <vector>

#include "knots/Socket.hpp"

class ThreadPool {
private:
    const int m_threadCount;
    std::atomic<bool> m_isRunning;

    // Container for functions that are considered "jobs"
    std::queue<std::function<void()>> m_jobs;

    // The main thread pool
    std::vector<std::jthread> m_threads;

    // Allows threads to wait on new jobs, or for termination
    std::condition_variable m_mutexCondition;

    // Mutex for m_jobs
    std::mutex m_jobsMutex;

    void ThreadLoop();

public:
    ThreadPool(const int threadCount);

    void EnqueueJob(const std::function<void()>& job);
    bool IsBusy();

    void Stop();
};