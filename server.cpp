#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ctime>
#include <sstream> 

#pragma comment(lib, "ws2_32.lib")  // Link with ws2_32.lib

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // Step 1: Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Step 2: Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 3: Set up the sockaddr_in structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Step 4: Bind the socket
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Step 5: Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    while (true) {
        // Step 6: Accept a client connection
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        

        std::cout << "Client connected!" << std::endl;

        // Step 7: Send a message to the client
        time_t timestamp = time(nullptr);
        std::string timeStr = ctime(&timestamp);  // Convert time to string
        timeStr.pop_back();
        std::stringstream responseStream;
        responseStream << "Hello Client. Your timestamp is " << timeStr << "\n";
        std::string response = responseStream.str();
        int sendResult = send(clientSocket, response.c_str(), response.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
        char buffer[BUFFER_SIZE] = {0};
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
        } else {
            buffer[bytesReceived] = '\0';  // Null-terminate the received data
            std::cout << "Message from client: " << buffer << std::endl;
        }

        // Step 8: Shutdown and close the client socket
        shutdown(clientSocket, SD_SEND);
        closesocket(clientSocket);
        std::cout << "Client disconnected. Waiting for new connections..." << std::endl;
    }

    // Step 9: Close the server socket and clean up (this part won't be reached unless you break the loop)
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
