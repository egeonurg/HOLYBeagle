#include <iostream>
#include <thread>
#include <chrono>
#include "TCPPackageHandler.h"

void sendLoop() 
{
    TCPPackageHandler sender(TCP_INTERFACE_NAME_ACTIVE);

    char arr[127];
    memset(arr,0xEE,sizeof(arr));
    while (true) 
    {
        sender.sendPacket("192.168.1.100", "192.168.1.1", arr);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait for 1 second
    }
}

int main() 
{
    std::thread senderThread(sendLoop);  // Start the thread

    senderThread.join();  // Keep the main thread alive
    return 0;
}
