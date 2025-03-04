#ifndef TCP_CROSS_COMPILE

#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <queue>
#include <mutex>
#include <condition_variable>

class TCPServerHandler {
private:
    int serverSocket;
    int clientSocket;
    std::queue<std::string> sendQueue;
    std::mutex mtx;
    std::condition_variable cv;
    bool running;

    void receiveData() {
        char buffer[1024];
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::cerr << "Client disconnected or error occurred!" << std::endl;
                running = false;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::cout << "Received: " << buffer << std::endl;
        }
    }

    void sendData() {
        while (running) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !sendQueue.empty() || !running; });

            if (!running && sendQueue.empty()) {
                break;
            }

            std::string message = sendQueue.front();
            sendQueue.pop();
            lock.unlock();

            if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
                std::cerr << "Failed to send data!" << std::endl;
                running = false;
                break;
            }

            std::cout << "Sent: " << message << std::endl;
        }
    }

public:
    TCPServerHandler() : serverSocket(-1), clientSocket(-1), running(true) {}

    void start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Failed to create socket!" << std::endl;
            return;
        }

        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << "Failed to set SO_REUSEADDR!" << std::endl;
            close(serverSocket);
            return;
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        inet_pton(AF_INET, "192.168.1.100", &serverAddress.sin_addr);

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to bind socket!" << std::endl;
            close(serverSocket);
            return;
        }

        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Failed to listen on socket!" << std::endl;
            close(serverSocket);
            return;
        }

        std::cout << "TCPServerHandler is listening on 192.168.1.100:8080..." << std::endl;

        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection!" << std::endl;
            close(serverSocket);
            return;
        }

        std::cout << "Client connected!" << std::endl;

        std::thread receiverThread(&TCPServerHandler::receiveData, this);
        std::thread senderThread(&TCPServerHandler::sendData, this);

        std::string input;
        while (running) {
            std::getline(std::cin, input);
            if (input == "exit") {
                running = false;
                break;
            }

            std::lock_guard<std::mutex> lock(mtx);
            sendQueue.push(input);
            cv.notify_one();
        }

        cv.notify_one();
        receiverThread.join();
        senderThread.join();

        close(clientSocket);
        close(serverSocket);
    }
};

#endif