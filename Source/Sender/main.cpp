#include <chrono>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include "../Common/static.h"

using namespace std;

int setSequenceNumber(char window[][SIZE], int j, int packetNum);
void transmitWindow(char window[][SIZE], int fd, struct sockaddr_in &addr);

int main(int argc, char **argv) {

    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SENDER_PORT);

    ifstream file((string(argv[1])));

    auto begin = chrono::high_resolution_clock::now();

    char data;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int currentPacketSize = 0, currentWindowSize = 0, packetNum = 0;
    char window[WINDOW_SIZE][SIZE] = {0};

    while (file >> noskipws >> data) {
        window[currentWindowSize][currentPacketSize++] = data;
        if (currentPacketSize == PACKET_SIZE) {
            packetNum = setSequenceNumber(window, currentWindowSize, packetNum);
            currentPacketSize = 0;
            currentWindowSize++;
        }
        if (currentWindowSize == WINDOW_SIZE) {
            transmitWindow(window, fd, serverAddr);
            currentWindowSize = 0;
        }
    }

    packetNum = setSequenceNumber(window, currentWindowSize, packetNum);

    while (currentWindowSize < WINDOW_SIZE) {
        currentWindowSize++;
        packetNum = setSequenceNumber(window, currentWindowSize, packetNum);
    }

    char endOfTransmit[EOF_DATA_SIZE];
    transmitWindow(window, fd, serverAddr);
    sprintf(endOfTransmit, "eof#%d", htons(RECEIVER_PORT));
    serverAddr.sin_port = htons(SENDER_PORT);

    sendto(fd, endOfTransmit, strlen(endOfTransmit), MSG_CONFIRM, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    auto end = chrono::high_resolution_clock::now();
    cout << "Transmitted in " << chrono::duration<double, milli>(end - begin).count() << " ms" << endl;

    close(fd);

    return 0;
}

void transmitWindow(char window[][SIZE], int fd, struct sockaddr_in &addr) {

    char msg[WINDOW_SIZE * 4];
    msg[0] = '\0';

    while (msg[0] != 'A') {

        for (int i = 0; i < WINDOW_SIZE; i++) {
            addr.sin_port = htons(SENDER_PORT);
            sendto(fd, window[i], strlen(window[i]), MSG_CONFIRM, (struct sockaddr *) &addr, sizeof(addr));
        }

        socklen_t len;
        struct timeval timeval{0, 1000};
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval));
        int n = recvfrom(fd, msg, PACKET_SIZE, MSG_WAITALL, (struct sockaddr *) &addr, &len);

        if (n < 0) {
            msg[0] = '\0';
            continue;
        }

        msg[n] = '\0';
    }

    for (int i = 0; i < WINDOW_SIZE; i++) memset(window[i], 0, SIZE);
}

int setSequenceNumber(char window[][SIZE], int j, int packetNum) {
    char src[EOF_DATA_SIZE];
    sprintf(src, "#%d#%d", packetNum, htons(RECEIVER_PORT));
    strncat(window[j], src, sizeof(src));
    return (packetNum + 1) % (WINDOW_SIZE * 2);
}
