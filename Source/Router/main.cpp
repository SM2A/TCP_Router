#include <queue>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include "../Common/common.h"

using namespace std;

void setSourcePort(char msg[], int port);

int main() {

    struct sockaddr_in serverAddr, clientAddr, receiverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    memset(&receiverAddr, 0, sizeof(receiverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SENDER_PORT);

    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    receiverAddr.sin_port = htons(RECEIVER_PORT);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    socklen_t len = sizeof(clientAddr);

    char *outMSG;
    char inMSG[SIZE];
    queue<char *> buffer;

    int receiverFD = socket(AF_INET, SOCK_DGRAM, 0);

    int n;
    while ((n = recvfrom(fd, inMSG, SIZE, MSG_WAITALL, (struct sockaddr *) &clientAddr, &len)) != -1) {

        inMSG[n] = '\0';
        if (buffer.size() < BUFFER_SIZE) buffer.push(inMSG);

        if (!buffer.empty()) {
            outMSG = buffer.front();
            int port = getNumber(outMSG);
            receiverAddr.sin_port = port;
            setSourcePort(outMSG, clientAddr.sin_port);
            sendto(receiverFD, outMSG, strlen(outMSG), MSG_CONFIRM, (struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
            buffer.pop();
        }
    }

    return 0;
}

void setSourcePort(char msg[], int port) {
    char *ch = new char[10];
    sprintf(ch, "#%d", port);
    strncat(msg, ch, sizeof(ch));
}
