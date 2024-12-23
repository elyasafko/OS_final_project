// ActiveObject.cpp
#include "ActiveObject.h"
#include <iostream>

ActiveObject::ActiveObject(int id) : stop(false), threadID(id)
{
    worker = std::thread(&ActiveObject::run, this);
}

ActiveObject::~ActiveObject()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }
    cv.notify_all();
    worker.join();
}

/**
 * @brief Adds a task to the task queue for the ActiveObject thread to execute.
 *
 * This function adds a task to the task queue and notifies the ActiveObject thread
 * that a new task is available. If the stop flag is set, it is not possible to enqueue
 * new tasks.
 *
 * @param task The task to enqueue.
 *
 * @exception std::runtime_error If the stop flag is set.
 */
void ActiveObject::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        tasks.push(task);
    }
    cv.notify_one();
}

/**
 * @brief The main loop of the ActiveObject thread.
 *
 * This function is where the thread spends most of its time. It waits for tasks to be
 * added to the queue and then executes them. If the stop flag is set and the queue is
 * empty, the thread exits.
 */
void ActiveObject::run()
{
    std::cout << "ActiveObject Thread " << threadID << " started.\n";
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]()
                    { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
            {
                std::cout << "ActiveObject Thread " << threadID << " stopping.\n";
                return;
            }
            task = tasks.front();
            tasks.pop();
        }
        std::cout << "ActiveObject Thread " << threadID << " executing task.\n";
        task();
    }
}
