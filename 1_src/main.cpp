#include "TCPClientHandler.h"
#include "TCPServerHandler.h"

int main() 
{
#ifdef TCP_CROSS_COMPILE
    TCPClientHandler client;
    client.start();
    return 0;
#else
    TCPServerHandler server;
    server.start();
    return 0;
#endif
}
