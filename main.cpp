// main.cpp
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "Server.h"
#include "ActiveObject.h"

const int PORT = 9034;

using namespace std;

// Define the ThreadPool and ActiveObject pointers
ThreadPool threadPool(4);

ActiveObject *stage1Pipeline;
ActiveObject *stage2Pipeline;
ActiveObject *stage3Pipeline;
ActiveObject *stage4Pipeline;

int main()
{
    // Create the server socket to listen for incoming connections
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr, clientAddr;
    if (serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    // Set the socket option to allow address reuse so that we can restart the server
    // without having to wait for the socket to timeout.
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address to listen on all available network interfaces
    // and listen on port 9034.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Bind the socket to the address and port.
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind");
        return 1;
    }

    // Start listening for incoming connections. The backlog parameter is set to 5
    // to allow up to 5 queued connections.
    if (listen(serverSocket, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    cout << "Server is running on port " << PORT << "..." << endl;

    // Initialize the pipeline stages.
    stage1Pipeline = new ActiveObject(1);
    stage2Pipeline = new ActiveObject(2);
    stage3Pipeline = new ActiveObject(3);
    stage4Pipeline = new ActiveObject(4);

    // Handle clients by accepting new connections and enqueuing the client handling
    // task to the thread pool.
    while (true)
    {
        socklen_t clientAddrSize = sizeof(sockaddr_in);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket < 0)
        {
            perror("accept");
            continue;
        }

        cout << "Accepted new client connection.\n";

        // Enqueue the client handling task to the thread pool. The client socket
        // is passed as an argument to the task.
        threadPool.enqueueTask([clientSocket]()
                               {
            int* clientSocketPtr = new int(clientSocket);
            handleClient((void*)clientSocketPtr); });
    }

    // Clean up (this code is unreachable unless the server is terminated)
    delete stage1Pipeline;
    delete stage2Pipeline;
    delete stage3Pipeline;
    delete stage4Pipeline;

    // Close the server socket.
    close(serverSocket);
    return 0;
}
