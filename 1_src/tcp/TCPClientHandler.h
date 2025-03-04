#ifdef TCP_CROSS_COMPILE

#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <mutex>
#include <condition_variable>

class Client {
private:
    int clientSocket;
    std::mutex mtx;
    bool running;

    void receiveData() {
        char buffer[1024];
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << "Server disconnected or error occurred!" << std::endl;
                running = false;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Received from server: " << buffer << std::endl;
        }
    }

    void sendData() {
        int messageCount = 0;
        while (running) {
            std::string message = "Queen of the castle is laying next to me, but she looks a bit pissed off. Are you pissed of Darina, tell me :) " + std::to_string(messageCount++);

            if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
                std::lock_guard<std::mutex> lock(mtx);
                std::cerr << "Failed to send data!" << std::endl;
                running = false;
                break;
            }

            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "Sent to server: " << message << std::endl;

            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

public:
    Client() : clientSocket(-1), running(true) {}

    void start() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Failed to create socket!" << std::endl;
            return;
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        inet_pton(AF_INET, "192.168.1.100", &serverAddress.sin_addr);

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to connect to server!" << std::endl;
            close(clientSocket);
            return;
        }

        std::cout << "Connected to server!" << std::endl;

        std::thread receiverThread(&Client::receiveData, this);
        std::thread senderThread(&Client::sendData, this);

        receiverThread.join();
        senderThread.join();

        close(clientSocket);
    }
};

#endif