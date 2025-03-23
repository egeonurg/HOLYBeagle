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

    void receiveData();
    void sendData();

public:
    Client();
    void start();
};

#endif