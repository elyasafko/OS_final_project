// Server.h
#ifndef SERVER_H
#define SERVER_H

#include "ThreadPool.h"
#include "ActiveObject.h"

extern ThreadPool threadPool;
extern ActiveObject* stage1Pipeline;
extern ActiveObject* stage2Pipeline;
extern ActiveObject* stage3Pipeline;
extern ActiveObject* stage4Pipeline;

void* handleClient(void* arg);
void computeMSTWithPipeline(int clientSocket, const std::string& algorithmName);
void computeMSTWithThreadPool(int clientSocket, const std::string& algorithmName);

#endif // SERVER_H
