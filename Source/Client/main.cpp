#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <afxres.h>
#include "../Packet/Packet.h"

using namespace std;

class Client {

private:

    int clientSocket;
    struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
    struct Packet udpSegment;

public:

    void createSocket();
    void setServerAddress(int port);
    void sendAck(Packet *packet);
};

void Client::createSocket() {
    if ((this->clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
}

void Client::setServerAddress(int port) {
    memset(&this->serverAddr, 0, sizeof(this->serverAddr));
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->serverAddr.sin_port = htons(port);
}

void Client::sendAck(Packet *packet) {
    Packet ackPacket;
    ackPacket.type = 0;
    ackPacket.acknum = packet->seqnum + 1;
    ackPacket.seqnum = packet->seqnum;
    int res = sendto(clientSocket, &ackPacket, sizeof(ackPacket), 0, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (res < 0) perror("sending ack failed");
    else cout << "ACK " << ackPacket.acknum << "sent for segment " << ackPacket.seqnum << endl;
}

int main() {
    cout << "Hello, World!" << endl;
    return 0;
}
