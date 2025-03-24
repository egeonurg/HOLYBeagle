#ifdef TCP_CROSS_COMPILE

#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <mutex>
#include <condition_variable>

class TCPClientHandler {
private:
    int clientSocket;
    std::mutex mtx;
    bool running;

    void receiveData();
    void sendData();

public:
    TCPClientHandler();
    void start();
};

#endif