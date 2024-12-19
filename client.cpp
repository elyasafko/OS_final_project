// client.cpp
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[4096];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9034);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cout << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "\nConnection Failed \n";
        return -1;
    }

    while (true)
    {
        // Clear buffer for next message
        std::memset(buffer, 0, sizeof(buffer));
        
        // Receive message from server
        ssize_t valread = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (valread <= 0)
        {
            std::cout << "Server closed the connection or error occurred.\n";
            break;
        }
        buffer[valread] = '\0';
        std::string serverMessage = buffer;
        // Display server message
        std::cout << serverMessage;


        if (serverMessage.find("Exiting") != std::string::npos)
        {
            break;
        }
        // Get user input
        std::string userInput;
        std::getline(std::cin, userInput);
        // Send user input to server
        userInput += "\n"; // Add newline character
        send(sock, userInput.c_str(), userInput.size(), 0);
    }

    close(sock);
    return 0;
}
