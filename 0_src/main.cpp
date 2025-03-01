#include <iostream>
#include <thread>
#include <chrono>
#include "TCPPackageHandler.h"

void sendLoop() 
{
    TCPPackageHandler sender("enp1s0");

    while (true) 
    {
        sender.sendPacket("192.168.1.100", "192.168.1.1",
                          "HELLOO I AM SENDING THIS PACKAGE TO MY DEAR DARLING DARINAAAAAAAAAAAAAAAA!");
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Wait for 1 second
    }
}

int main() {
    std::thread senderThread(sendLoop);  // Start the thread

    senderThread.join();  // Keep the main thread alive
    return 0;
}
