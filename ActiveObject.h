// ActiveObject.h
#ifndef ACTIVEOBJECT_H
#define ACTIVEOBJECT_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ActiveObject
{
public:
    ActiveObject(int id); // Constructor with thread ID
    ~ActiveObject();
    void enqueue(std::function<void()> task);

private:
    void run();
    std::thread worker;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    bool stop;
    int threadID; // Thread identifier
};

#endif // ACTIVEOBJECT_H
