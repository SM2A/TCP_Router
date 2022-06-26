#include <queue>
#include <cstring>
#include <iostream>
#include <netinet/in.h>

using namespace std;

#define SENDER_PORT 1398
#define RECEIVER_PORT 1401

#define WINDOW_SIZE 4
#define BUFFER_SIZE 10
#define PACKET_SIZE 1500

void addSourcePort(char *message, const int port) {
    char *ch = new char[10];
    sprintf(ch, "_%d", port);
    strncat(message, ch, sizeof(ch));
}

int getDestPort(char *message) {
    int num = 0, j = 1;
    for (int i = strlen(message) - 1; i >= 0; i--) {
        if (message[i] == '_') {
            message[i] = '\0';
            return num;
        }
        num += (int) (message[i] - '0') * j;
        j *= 10;
    }
    return -1;
}

int main() {

    queue<char *> buffer;
    char *to_send_message;
    char message[PACKET_SIZE + WINDOW_SIZE * 2 + 12];

    struct sockaddr_in serverAddr, clientAddr, receiverAddr;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int receiverSock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
    memset(&receiverAddr, 0, sizeof(receiverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SENDER_PORT);

    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_addr.s_addr = INADDR_ANY;
    receiverAddr.sin_port = htons(RECEIVER_PORT);

    bind(fd, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));

    socklen_t len = sizeof(clientAddr);

    int n;
    while ((n = recvfrom(fd, (char *) message, PACKET_SIZE + WINDOW_SIZE * 2 + 12, MSG_WAITALL, (struct sockaddr *) &clientAddr, &len)) != -1) {

        message[n] = '\0';
        if (buffer.size() < BUFFER_SIZE) buffer.push(message);

        if (!buffer.empty()) {
            to_send_message = buffer.front();
            buffer.pop();
            int port = getDestPort(to_send_message);
            receiverAddr.sin_port = port;
            addSourcePort(to_send_message, clientAddr.sin_port);
            sendto(receiverSock, (const char *) to_send_message, strlen(to_send_message),MSG_CONFIRM,
                   (const struct sockaddr *) &receiverAddr, sizeof(receiverAddr));
        }
    }

    return 0;
}
