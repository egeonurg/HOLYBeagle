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
#ifdef CROSS_COMPILE_BBBI
sender.sendPacket("192.168.8.94", "192.168.0.100", arr);
#else
sender.sendPacket("192.168.0.100", "192.168.8.94", arr);
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait for 1 second
    }
}

int main() 
{
    std::thread senderThread(sendLoop);  // Start the thread

    senderThread.join();  // Keep the main thread alive
    return 0;
}
