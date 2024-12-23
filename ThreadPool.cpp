// ThreadPool.cpp
#include "ThreadPool.h"
#include <iostream>

/**
 * @brief Creates a ThreadPool with a specified number of threads.
 *
 * The constructor creates a number of worker threads and starts them. The
 * threads are waiting for tasks to be enqueued and will execute them
 * sequentially.
 *
 * @param threads The number of worker threads to create.
 */
ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back([this, i]
                             {
            while(true) 
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);

                    // Wait until either the pool is stopped or there is a task to process
                    this->condition.wait(lock,
                        [this]{ return this->stop || !this->tasks.empty(); });

                    if(this->stop && this->tasks.empty())
                    {
                        // If the pool is stopped and there are no tasks left, exit the loop
                        return;
                    }

                    // This thread becomes the leader and dequeues the task
                    std::cout << "Thread " << i << " is the leader.\n";

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // Execute the task
                task();

                // After completing, promote a follower to leader
                std::cout << "Thread " << i << " completed task and is promoting a new leader.\n";
                this->condition.notify_one();  // Notify next follower
            } });
    }
}

/**
 * @brief Enqueues a task for execution by one of the worker threads.
 *
 * This function adds a task to the task queue and notifies one waiting
 * thread that a new task is available. If the pool is stopped, it is not
 * possible to enqueue new tasks.
 *
 * @param task The task to enqueue.
 *
 * @exception std::runtime_error If the pool is stopped.
 */
void ThreadPool::enqueueTask(std::function<void()> task)
{
    // Acquire a lock to ensure thread safety while modifying the task queue
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // If the pool is stopped, it is not possible to enqueue new tasks
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // Move the task into the task queue
        tasks.push(std::move(task));
    }

    // Notify one waiting thread that a new task is available
    condition.notify_one();
}

/**
 * @brief Stops the ThreadPool and waits for all threads to finish.
 *
 * This destructor sets a stop flag to signal to all threads that they should stop
 * and notifies all waiting threads that the pool is stopped and that there may be
 * tasks available to process. This will cause all threads to exit the loop.
 * Finally, it waits for all threads to finish by calling join on each thread.
 */
ThreadPool::~ThreadPool()
{
    // Acquire a lock to ensure thread safety when stopping the thread pool
    std::unique_lock<std::mutex> lock(queueMutex);

    // Set the stop flag to true to signal to all threads that they should stop
    stop = true;

    // Notify all waiting threads that the pool is stopped and that there may be tasks
    // available to process. This will cause all threads to exit the loop.
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    // Wait for all threads to finish
    for (std::thread &worker : workers)
    {
        // Join each thread to wait for it to finish
        worker.join();
    }
}
