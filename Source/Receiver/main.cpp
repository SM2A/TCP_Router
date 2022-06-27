#include <cstring>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include "../Common/common.h"

using namespace std;

void sendAck(int packetNum, int fd, struct sockaddr_in &addr, int port);

int main(int argc, char **argv) {

    struct sockaddr_in servAddr, clientAddr;

    memset(&servAddr, 0, sizeof(servAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(RECEIVER_PORT);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(fd, (struct sockaddr *) &servAddr, sizeof(servAddr));

    socklen_t len = sizeof(clientAddr);

    ofstream file((string(argv[1])));

    int expectedPacketNum = 0;
    char message[WINDOW_SIZE][SIZE];
    bool receivedPackets[WINDOW_SIZE];

    memset(receivedPackets, false, WINDOW_SIZE);

    while (true) {

        char packet[SIZE];
        int n = recvfrom(fd, packet, SIZE, MSG_WAITALL, (struct sockaddr *) &clientAddr, &len);
        packet[n] = '\0';
        int port = getNumber(packet);

        if (!strcmp(packet, "eof")) break;

        int packetNum = getNumber(packet);
        if ((expectedPacketNum + WINDOW_SIZE > packetNum) && (expectedPacketNum <= packetNum)) {
            strncpy(message[packetNum % WINDOW_SIZE], packet, SIZE);
            receivedPackets[packetNum % WINDOW_SIZE] = true;
        } else sendAck(expectedPacketNum, fd, clientAddr, port);

        bool receivedAll = true;
        for (bool receivedPacket : receivedPackets) if (!receivedPacket) receivedAll = false;

        if (receivedAll) {
            memset(receivedPackets, false, WINDOW_SIZE);
            for (auto &i : message) file << i;
            expectedPacketNum = (expectedPacketNum + WINDOW_SIZE) % (2 * WINDOW_SIZE);
            sendAck(expectedPacketNum, fd, clientAddr, port);
        }
    }

    file.close();

    return 0;
}

void sendAck(int packetNum, int fd, struct sockaddr_in &addr, int port) {
    char msg[20];
    addr.sin_port = htons(SENDER_PORT);
    sprintf(msg, "ACK-%d#%d", packetNum, port);
    sendto(fd, msg, strlen(msg), MSG_CONFIRM, (struct sockaddr *) &addr, sizeof(addr));
}
