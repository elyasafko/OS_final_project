// Server.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <algorithm>
#include <thread>

#include "Server.h"
#include "Graph.h"
#include "MSTFactory.h"
#include "Measurements.h"
#include "ActiveObject.h"
#include "ThreadPool.h"

using namespace std;

// Global graph object and mutex
Graph *g = nullptr;
pthread_mutex_t graphMutex = PTHREAD_MUTEX_INITIALIZER;

// Global variables for threading models
extern ThreadPool threadPool;
extern ActiveObject *stage1Pipeline;
extern ActiveObject *stage2Pipeline;
extern ActiveObject *stage3Pipeline;
extern ActiveObject *stage4Pipeline;

// Function prototypes
void sendMenu(int clientSocket);
void processClientInput(int clientSocket, const string &input);

// Function definitions

/**
 * @brief Computes MST using the Pipeline threading model.
 * @param clientSocket The client's socket descriptor.
 * @param algorithmName The name of the MST algorithm to use ("Prim" or "Kruskal").
 */
void computeMSTWithPipeline(int clientSocket, const string &algorithmName)
{
    // Enqueue the initial task to Stage 1
    stage1Pipeline->enqueue([clientSocket, algorithmName]()
                            {
                                // Stage 1: Parsing Stage
                                cout << "[Pipeline] Stage 1: Parsing command on Thread "
                                     << this_thread::get_id() << ".\n";

                                // Copy the algorithm name for use in the next stage
                                string algName = algorithmName;

                                // Pass to Stage 2
                                stage2Pipeline->enqueue([clientSocket, algName]()
                                                        {
                                                            // Stage 2: Computation Stage - Compute MST
                                                            cout << "[Pipeline] Stage 2: Computing MST using " << algName
                                                                 << " on Thread " << this_thread::get_id() << ".\n";

                                                            // Lock the mutex to safely access the shared graph object
                                                            pthread_mutex_lock(&graphMutex);

                                                            // Create the MST algorithm instance based on the provided name
                                                            auto mstAlgorithm = MSTFactory::createAlgorithm(algName);

                                                            // Compute the Minimum Spanning Tree (MST) and get the computation log
                                                            auto mstEdges = mstAlgorithm->computeMST(*g);
                                                            string computationLog = mstAlgorithm->getComputationLog();

                                                            // Unlock the mutex after accessing the graph
                                                            pthread_mutex_unlock(&graphMutex);

                                                            // Pass to Stage 3 - Measurements
                                                            stage3Pipeline->enqueue([clientSocket, algName, mstEdges, computationLog]()
                                                                                    {
                                                                                        // Stage 3: Measurement Stage
                                                                                        cout << "[Pipeline] Stage 3: Calculating measurements on Thread "
                                                                                             << this_thread::get_id() << ".\n";

                                                                                        // Calculate the total weight of the MST
                                                                                        double totalWeight = calculateTotalWeight(mstEdges);

                                                                                        // Build a graph representation of the MST
                                                                                        Graph mstGraph = buildMSTGraph(g->getNumVertices(), mstEdges);

                                                                                        // Calculate the longest and shortest distances in the MST
                                                                                        auto distances = calculateDistancesInMST(mstGraph);

                                                                                        // Calculate the average distance in the original graph
                                                                                        double averageDistance = calculateAverageDistance(*g);

                                                                                        // Pass to Stage 4 - Response
                                                                                        stage4Pipeline->enqueue([clientSocket, algName, totalWeight, distances, averageDistance, computationLog]()
                                                                                                                {
                                                                                                                    // Stage 4: Response Stage
                                                                                                                    cout << "[Pipeline] Stage 4: Sending response on Thread "
                                                                                                                         << this_thread::get_id() << ".\n";

                                                                                                                    // Prepare the result message with separators
                                                                                                                    stringstream result;
                                                                                                                    result << "\n==== Computation Result ====\n";
                                                                                                                    result << "Computed using " << algName << " algorithm with Pipeline pattern:\n";
                                                                                                                    result << "Total Weight of MST: " << totalWeight << "\n";
                                                                                                                    result << "Longest Distance in MST: " << distances.first << "\n";
                                                                                                                    result << "Shortest Distance in MST: " << distances.second << "\n";
                                                                                                                    result << "Average Distance in Graph: " << averageDistance << "\n";
                                                                                                                    result << "\nComputation Steps:\n"
                                                                                                                           << computationLog;
                                                                                                                    result << "============================\n\n";
                                                                                                                    result << "Please select an option:\n"
                                                                                                                              "1) Create a new graph\n"
                                                                                                                              "2) Add an edge\n"
                                                                                                                              "3) Remove an edge\n"
                                                                                                                              "4) Compute MST\n"
                                                                                                                              "5) Exit\n"
                                                                                                                              "Enter your choice: \n";
                                                                                                                    // Send the result to the client
                                                                                                                    send(clientSocket, result.str().c_str(), result.str().size(), 0);

                                                                                                                    // Send the menu again to the client
                                                                                                                }); // End of Stage 4
                                                                                    });                             // End of Stage 3
                                                        });                                                         // End of Stage 2
                            });                                                                                     // End of Stage 1
}

/**
 * @brief Computes MST using the Leader-Follower threading model with a thread pool.
 * @param clientSocket The client's socket descriptor.
 * @param algorithmName The name of the MST algorithm to use ("Prim" or "Kruskal").
 *
 * This function is a bit tricky, so I'll explain what it does:
 *
 * 1. It takes a client socket and an algorithm name as arguments.
 * 2. It creates an MST algorithm using the provided algorithm name.
 * 3. It locks a mutex (graphMutex) so that only one thread can access the graph at a time.
 * 4. It computes the MST using the selected algorithm and logs the computation steps.
 * 5. It performs some measurements on the MST (total weight, longest and shortest distances, average distance).
 * 6. It prepares a response string that includes the measurements and the computation steps.
 * 7. It sends the response string and the main menu to the client.
 * 8. If an exception is thrown during the computation, it catches it, unlocks the mutex, and sends an error message to the client.
 *
 * To achieve this, it enqueues the computation task to the thread pool, which will execute the task on one of its threads.
 * This allows multiple clients to be handled concurrently.
 */
void computeMSTWithThreadPool(int clientSocket, const string &algorithmName)
{
    // Enqueue the computation task to the thread pool
    threadPool.enqueueTask([clientSocket, algorithmName]()
                           {
        cout << "[ThreadPool] Computing MST using " << algorithmName
             << " on Thread " << this_thread::get_id() << ".\n";

        // Lock the mutex to ensure that only one thread can access the graph at a time
        pthread_mutex_lock(&graphMutex);
        try {
            // Create an MST algorithm using the provided algorithm name
            auto mstAlgorithm = MSTFactory::createAlgorithm(algorithmName);

            // Compute MST and log steps
            auto mstEdges = mstAlgorithm->computeMST(*g);
            string computationLog = mstAlgorithm->getComputationLog();

            // Perform measurements
            double totalWeight = calculateTotalWeight(mstEdges);
            Graph mstGraph = buildMSTGraph(g->getNumVertices(), mstEdges);
            auto distances = calculateDistancesInMST(mstGraph);
            double averageDistance = calculateAverageDistance(*g);

            // Unlock the mutex so that other threads can access the graph
            pthread_mutex_unlock(&graphMutex);

            // Prepare the result with separators
            stringstream result;
            result << "\n==== Computation Result ====\n";
            result << "Computed using " << algorithmName << " algorithm with Leader-Follower Thread Pool:\n";
            result << "Total Weight of MST: " << totalWeight << "\n";
            result << "Longest Distance in MST: " << distances.first << "\n";
            result << "Shortest Distance in MST: " << distances.second << "\n";
            result << "Average Distance in Graph: " << averageDistance << "\n";
            result << "\nComputation Steps:\n" << computationLog;
            result << "============================\n\n";
            result << "Please select an option:\n"
                        "1) Create a new graph\n"
                        "2) Add an edge\n"
                        "3) Remove an edge\n"
                        "4) Compute MST\n"
                        "5) Exit\n"
                        "Enter your choice: \n";
            // Send the result to the client
            send(clientSocket, result.str().c_str(), result.str().size(), 0);
            cout << "[ThreadPool] Sent computation result to client.\n";
        } catch (const exception& e) {
            // Unlock the mutex if an exception is thrown
            pthread_mutex_unlock(&graphMutex);
            string errorMsg = string("Error: ") + e.what() + "\n";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            sendMenu(clientSocket);
        } });
}

/**
 * @brief Handles communication with a connected client.
 * @param arg Pointer to the client's socket descriptor.
 * @return nullptr
 */
void *handleClient(void *arg)
{
    // Retrieve the socket descriptor from the argument and delete the pointer
    int clientSocket = *(int *)arg;
    delete (int *)arg;

    // Buffer to store incoming data from the client
    char buffer[4096];
    // String to store the line received from the client
    string line;

    // Send the menu
    sendMenu(clientSocket);

    // Continuously handle client requests until disconnection or error
    while (true)
    {
        // Receive input from the client into the buffer
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        // Check if the client has disconnected or an error occurred
        if (bytesReceived <= 0)
        {
            // Inform that the client has disconnected or an error has occurred
            cout << "Client disconnected or error occurred.\n";
            // Close the client socket to release resources
            close(clientSocket);
            return nullptr;
        }

        // Null-terminate the buffer to create a proper C-style string
        buffer[bytesReceived] = '\0';
        // Convert the buffer into a C++ string for easier processing
        line = buffer;

        // Process the input received from the client
        processClientInput(clientSocket, line);
    }

    // Close the client socket (this line is unreachable due to the infinite loop)
    close(clientSocket);
    return nullptr;
}

/**
 * @brief Sends the main menu to the client.
 * @param clientSocket The client's socket descriptor.
 */
void sendMenu(int clientSocket)
{
    string menu = "Please select an option:\n"
                  "1) Create a new graph\n"
                  "2) Add an edge\n"
                  "3) Remove an edge\n"
                  "4) Compute MST\n"
                  "5) Exit\n"
                  "Enter your choice: \n";
    send(clientSocket, menu.c_str(), menu.size(), 0);
}

/**
 * @brief Processes input received from the client.
 * @param clientSocket The client's socket descriptor.
 * @param input The input received from the client.
 */
void processClientInput(int clientSocket, const string &input)
{
    // Static variables to maintain state between function calls
    static int state = 0;                        // Tracks the current state of input processing
    static int n = 0, m = 0, edgeCount = 0;      // Graph parameters and edge count
    static stringstream ss;                      // Stringstream for parsing input
    static string algorithmName, threadingModel; // Selected algorithm and threading model

    istringstream iss(input); // Create input string stream
    string command = input;
    // Remove newline character from the command
    command.erase(remove(command.begin(), command.end(), '\n'), command.end());

    switch (state)
    {
    case 0:
    { // Waiting for main menu choice
        int choice;
        try
        {
            choice = stoi(command); // Convert command to integer choice
        }
        catch (...)
        {
            // Handle invalid input by notifying the client and resending the menu
            string errorMsg = "Invalid choice. Please try again.\n";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            sendMenu(clientSocket);
            return;
        }

        // Process the menu choice
        if (choice == 1)
        {
            // Prompt for number of vertices if creating a new graph
            string prompt = "Enter number of vertices (n): ";
            send(clientSocket, prompt.c_str(), prompt.size(), 0);
            state = 1; // Change state to expect number of vertices
        }
        else if (choice == 2)
        {
            // Prompt for edge details if adding an edge
            string prompt = "Enter edge to add (src dest weight): ";
            send(clientSocket, prompt.c_str(), prompt.size(), 0);
            state = 4; // Change state to expect edge details
        }
        else if (choice == 3)
        {
            // Prompt for edge details if removing an edge
            string prompt = "Enter edge to remove (src dest): ";
            send(clientSocket, prompt.c_str(), prompt.size(), 0);
            state = 5; // Change state to expect edge details
        }
        else if (choice == 4)
        {
            // Prompt to select MST algorithm
            string prompt = "Select the algorithm:\n"
                            "1) Prim\n"
                            "2) Kruskal\n"
                            "Enter your choice: ";
            send(clientSocket, prompt.c_str(), prompt.size(), 0);
            state = 6; // Change state to expect algorithm choice
        }
        else if (choice == 5)
        {
            // Exit the connection
            string msg = "Exiting...\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
            close(clientSocket);   // Close client connection
            pthread_exit(nullptr); // Exit the thread
        }
        else
        {
            // Handle invalid choice by notifying the client and resending the menu
            string errorMsg = "Invalid choice. Please try again.\n";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            sendMenu(clientSocket);
        }
        break;
    }
    case 1:
    { // Received number of vertices
        try
        {
            n = stoi(command); // Convert command to number of vertices
        }
        catch (...)
        {
            // Handle invalid input by notifying the client and prompting again
            string errorMsg = "Invalid number. Please enter number of vertices (n): ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        // Prompt for number of edges
        string prompt = "Enter number of edges (m): ";
        send(clientSocket, prompt.c_str(), prompt.size(), 0);
        state = 2; // Change state to expect number of edges
        break;
    }
    case 2:
    { // Received number of edges
        try
        {
            m = stoi(command); // Convert command to number of edges
        }
        catch (...)
        {
            // Handle invalid input by notifying the client and prompting again
            string errorMsg = "Invalid number. Please enter number of edges (m): ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        // Initialize the graph with the specified number of vertices
        pthread_mutex_lock(&graphMutex);
        delete g;         // Delete existing graph if any
        g = new Graph(n); // Create new graph
        pthread_mutex_unlock(&graphMutex);

        // Prompt for edge details in specific format
        string prompt = "Enter edges in format: src dest weight (x x x.x)\n";
        prompt += "Edge 0: ";
        send(clientSocket, prompt.c_str(), prompt.size(), 0);
        edgeCount = 0; // Reset edge count
        state = 3;     // Change state to collect edges
        break;
    }
    case 3:
    { // Collecting edges
        int src, dest;
        double weight;
        istringstream edgeStream(command); // Parse edge details
        if (!(edgeStream >> src >> dest >> weight))
        {
            // Handle invalid edge format by notifying the client and prompting again
            string errorMsg = "Invalid edge format. Please enter edge (src dest weight (x x x.x)): ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        pthread_mutex_lock(&graphMutex);
        g->addEdge(src, dest, weight); // Add edge to graph
        pthread_mutex_unlock(&graphMutex);
        edgeCount++; // Increment edge count
        if (edgeCount < m)
        {
            // Prompt for the next edge if more are expected
            string prompt = "Edge " + to_string(edgeCount) + ": ";
            send(clientSocket, prompt.c_str(), prompt.size(), 0);
        }
        else
        {
            sendMenu(clientSocket);
            state = 0; // Reset state to wait for the next main menu choice
        }
        break;
    }
    case 4:
    { // Add an edge
        int src, dest;
        double weight;
        istringstream edgeStream(command); // Parse edge details
        if (!(edgeStream >> src >> dest >> weight))
        {
            // Handle invalid edge format by notifying the client and prompting again
            string errorMsg = "Invalid edge format. Please enter edge to add (src dest weight): ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        pthread_mutex_lock(&graphMutex);
        if (g)
        {
            g->addEdge(src - 1, dest - 1, weight); // Add edge to graph
            string msg = "Edge added.\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
        else
        {
            string msg = "No graph created yet.\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
        pthread_mutex_unlock(&graphMutex);
        sendMenu(clientSocket); // Resend menu
        state = 0;              // Reset state
        break;
    }
    case 5:
    { // Remove an edge
        int src, dest;
        istringstream edgeStream(command); // Parse edge details
        if (!(edgeStream >> src >> dest))
        {
            // Handle invalid edge format by notifying the client and prompting again
            string errorMsg = "Invalid edge format. Please enter edge to remove (src dest): ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        pthread_mutex_lock(&graphMutex);
        if (g)
        {
            g->removeEdge(src - 1, dest - 1); // Remove edge from graph
            string msg = "Edge removed.\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
        else
        {
            string msg = "No graph created yet.\n";
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
        pthread_mutex_unlock(&graphMutex);
        sendMenu(clientSocket); // Resend menu
        state = 0;              // Reset state
        break;
    }
    case 6:
    { // Select algorithm
        int algChoice;
        try
        {
            algChoice = stoi(command); // Convert command to algorithm choice
        }
        catch (...)
        {
            // Handle invalid choice by notifying the client and prompting again
            string errorMsg = "Invalid choice. Select the algorithm:\n"
                              "1) Prim\n"
                              "2) Kruskal\n"
                              "Enter your choice: ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        if (algChoice == 1)
        {
            algorithmName = "Prim"; // Set algorithm to Prim if chosen
        }
        else if (algChoice == 2)
        {
            algorithmName = "Kruskal"; // Set algorithm to Kruskal if chosen
        }
        else
        {
            // Handle invalid choice by notifying the client and prompting again
            string errorMsg = "Invalid choice. Select the algorithm:\n"
                              "1) Prim\n"
                              "2) Kruskal\n"
                              "Enter your choice: ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        // Prompt to select threading model
        string prompt = "Select the threading model:\n"
                        "1) Pipeline\n"
                        "2) Leader-Follower\n"
                        "Enter your choice: ";
        send(clientSocket, prompt.c_str(), prompt.size(), 0);
        state = 7; // Change state to expect threading model choice
        break;
    }
    case 7:
    { // Select threading model
        int threadingChoice;
        try
        {
            threadingChoice = stoi(command); // Convert command to threading model choice
        }
        catch (...)
        {
            // Handle invalid choice by notifying the client and prompting again
            string errorMsg = "Invalid choice. Select the threading model:\n"
                              "1) Pipeline\n"
                              "2) Leader-Follower\n"
                              "Enter your choice: ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        if (threadingChoice == 1)
        {
            threadingModel = "Pipeline"; // Set threading model to Pipeline if chosen
        }
        else if (threadingChoice == 2)
        {
            threadingModel = "LeaderFollower"; // Set threading model to Leader-Follower if chosen
        }
        else
        {
            // Handle invalid choice by notifying the client and prompting again
            string errorMsg = "Invalid choice. Select the threading model:\n"
                              "1) Pipeline\n"
                              "2) Leader-Follower\n"
                              "Enter your choice: ";
            send(clientSocket, errorMsg.c_str(), errorMsg.size(), 0);
            return;
        }
        // Compute MST using the selected algorithm and threading model
        pthread_mutex_lock(&graphMutex);
        if (g)
        {
            pthread_mutex_unlock(&graphMutex);
            if (threadingModel == "Pipeline")
            {
                // Perform computation using the Pipeline pattern
                computeMSTWithPipeline(clientSocket, algorithmName);
            }
            else if (threadingModel == "LeaderFollower")
            {
                // Perform computation using the Leader-Follower Thread Pool
                computeMSTWithThreadPool(clientSocket, algorithmName);
            }
            state = 0; // Reset state to wait for the next main menu choice
        }
        else
        {
            pthread_mutex_unlock(&graphMutex);

            sendMenu(clientSocket);
            state = 0; // Reset state
        }
        break;
    }
    default:
    {
        // Reset state and send menu in case of unexpected state
        state = 0;
        sendMenu(clientSocket);
        break;
    }
    }
}
