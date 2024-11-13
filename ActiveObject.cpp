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

void ActiveObject::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        tasks.push(task);
    }
    cv.notify_one();
}

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
