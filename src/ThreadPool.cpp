#include <mutex>

#include "knots/ThreadPool.hpp"

/*
    @brief Constructor for the ThreadPool class
    @param threadCount Number of threads to spin up in the pool
*/
ThreadPool::ThreadPool(const int threadCount) :
    m_threadCount(threadCount),
    m_isRunning(true) {

    // Spin up the threads
    for (int i = 0; i < m_threadCount; i++) {
        m_threads.emplace_back(std::jthread(&ThreadPool::ThreadLoop, this));
    }

    return;
}


/*
    @brief Infinite loop function that waits for jobs in m_jobs
*/
void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_jobsMutex);

            m_mutexCondition.wait(lock, [this] {
                return m_jobs.empty() == false || m_isRunning == false;
            });

            if (m_isRunning == false) {
                return;
            }

            job = m_jobs.front();
            m_jobs.pop();
        }

        job();
    }

    return;
}


/*
    @brief Enqueue the job in m_jobs
    @param job The function to execute, of signature `void ()`
*/
void ThreadPool::EnqueueJob(const std::function<void()>& job) {
    {
        std::scoped_lock<std::mutex> lock(m_jobsMutex);
        m_jobs.push(job);
    }

    m_mutexCondition.notify_one();
    return;
}


/*
    @brief Check if the thread pool is busy

    @return `true` if it's busy, `false` if it's not

    @note The pool is "busy" if there is even one job in the m_jobs
*/
bool ThreadPool::IsBusy() {
    bool isBusy = false;
    {
        std::scoped_lock<std::mutex> lock(m_jobsMutex);
        isBusy = m_jobs.empty() == false;
    }

    return isBusy;
}

/*
    @brief Stop the thread pool
    Joins all the threads
*/
void ThreadPool::Stop() {
    m_isRunning = false;

    m_mutexCondition.notify_all();
    m_threads.clear();
}