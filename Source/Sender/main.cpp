#include <chrono>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

#define SENDER_PORT 1398
#define RECEIVER_PORT 1401

#define WINDOW_SIZE 4
#define PACKET_SIZE 1500

int addSequenceNumber(char window[][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int j, int packet_num);
void transmitWindow(char window[][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int fd, struct sockaddr_in &addr);

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
    int currentPacketSize = 0, currentWindowSize = 0, packet_num = 0;
    char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12] = {0};

    while (file >> noskipws >> data) {
        window[currentWindowSize][currentPacketSize++] = data;
        if (currentWindowSize == WINDOW_SIZE) {
            transmitWindow(window, fd, serverAddr);
            currentWindowSize = 0;
        }
        if (currentPacketSize == PACKET_SIZE) {
            packet_num = addSequenceNumber(window, currentWindowSize, packet_num);
            currentPacketSize = 0;
            currentWindowSize++;
        }
    }

    packet_num = addSequenceNumber(window, currentWindowSize, packet_num);

    while (currentWindowSize < WINDOW_SIZE) {
        currentWindowSize++;
        packet_num = addSequenceNumber(window, currentWindowSize, packet_num);
    }

    char endOfTransmit[12];
    transmitWindow(window, fd, serverAddr);
    sprintf(endOfTransmit, "eof#%d", htons(RECEIVER_PORT));
    serverAddr.sin_port = htons(SENDER_PORT);

    sendto(fd, endOfTransmit, 12, MSG_CONFIRM, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    auto end = chrono::high_resolution_clock::now();
    cout << "Transmitted in " << chrono::duration<double, milli>(end - begin).count() << " ms" << endl;

    close(fd);

    return 0;
}

void transmitWindow(char window[][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int fd, struct sockaddr_in &addr) {

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

    for (int k = 0; k < WINDOW_SIZE; k++) memset(window[k], 0, PACKET_SIZE + WINDOW_SIZE * 2 + 12);
}

int addSequenceNumber(char window[][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int j, int packet_num) {
    char src[12];
    sprintf(src, "#%d#%d", packet_num, htons(RECEIVER_PORT));
    strncat(window[j], src, sizeof(src));
    return (packet_num + 1) % (WINDOW_SIZE * 2);
}
