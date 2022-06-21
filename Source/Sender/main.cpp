#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <cstring>
#include <vector>
#include "Packet/Packet.h"
using namespace std;

class Sender
{
private:
    int socketFd;
    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;

public:
    void createSocket();

};

void Sender::createSocket()
{
    if ((this->socketFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    return 0;
}