#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <cstring>
#include <vector>
#include "../Packet/Packet.h"
using namespace std;

class Receiver
{
private:
    int socketFd;
    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
    struct Packet udpSegment;
    struct Packet ackPacket;
    vector<Packet> buffer;

public:
    void createSocket();
    void setServerAddress(int port);
    void sendAck(Packet packet);
    void receive(int windowSize);

private:
    struct Packet createAck(int type, int ackNum);
};

void Receiver::createSocket()
{
    if ((this->socketFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
}

void Receiver::setServerAddress(int port)
{
    memset(&this->serverAddr, 0, sizeof(this->serverAddr));
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->serverAddr.sin_port = htons(port);
}

void Receiver::receive(int windowSize=1)
{   
    int expectedSeqNum = 0;

    for(;;)
    {

        unsigned int clientAddrLen = sizeof(clientAddr);
        int n_bytes = recvfrom(socketFd, &udpSegment, sizeof(udpSegment), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if(n_bytes < 0) {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        if (udpSegment.seqnum == expectedSeqNum)
        {
            buffer.push_back(udpSegment);
            expectedSeqNum++;
            ackPacket = createAck(1, expectedSeqNum);
            sendAck(ackPacket);
        }

    }
}

struct Packet Receiver::createAck(int type, int ackNum)
{
    struct Packet ackPacket;
    ackPacket.type = type;
    ackPacket.seqnum = ackNum;
    memset(ackPacket.data, 0, sizeof(ackPacket.data));
    return ackPacket;
}

void Receiver::sendAck(Packet packet)
{
    int res = sendto(socketFd, &packet, sizeof(packet), 0, (const struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(res < 0)
        perror("sending ack failed");
    else 
        cout << "ACK " << packet.seqnum << "sent" << endl;
}


int main()
{
    // Instantiate a Receiver
    return 0;
}
