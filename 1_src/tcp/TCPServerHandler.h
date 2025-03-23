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

    void receiveData();
    void sendData();

public:
    TCPServerHandler();
    void start();
};

#endif