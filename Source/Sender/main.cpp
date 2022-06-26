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

void sendWindowPackets(char window[][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int fd, struct sockaddr_in &addr) {

    char message[WINDOW_SIZE * 4];
    message[0] = '\0';

    while (message[0] != 'A') {

        for (int k = 0; k < WINDOW_SIZE; k++) {
            addr.sin_port = htons(SENDER_PORT);
            sendto(fd, (const char *) window[k], strlen(window[k]), MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
        }

        socklen_t len;
        struct timeval tv{0, 1000};
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        int n = recvfrom(fd, (char *) message, PACKET_SIZE, MSG_WAITALL, (struct sockaddr *) &addr, &len);

        if (n < 0) {
            message[0] = '\0';
            continue;
        }

        message[n] = '\0';
    }

    for (int k = 0; k < WINDOW_SIZE; k++) memset(window[k], 0, PACKET_SIZE + WINDOW_SIZE * 2 + 12);
}

int addPacketNum(char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int j, int packet_num) {
    char *ch = new char[12];
    sprintf(ch, "_%d_%d", packet_num, htons(RECEIVER_PORT));
    strncat(window[j], ch, sizeof(ch));
    return (packet_num + 1) % (WINDOW_SIZE * 2);
}

int main() {

    char character;
    int i = 0, j = 0, packet_num = 0, fd = socket(AF_INET, SOCK_DGRAM, 0);
    char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12] = {0};

    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SENDER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    fstream file("../Data/Data.txt");
    auto begin = chrono::high_resolution_clock::now();

    while (file >> noskipws >> character) {
        window[j][i++] = character;
        if (i == 10) {
            packet_num = addPacketNum(window, j, packet_num);
            i = 0;
            j++;
        }
        if (j == WINDOW_SIZE) {
            sendWindowPackets(window, fd, serverAddr);
            j = 0;
        }
    }

    packet_num = addPacketNum(window, j, packet_num);

    while (j < WINDOW_SIZE) {
        j++;
        packet_num = addPacketNum(window, j, packet_num);
    }

    char *endChar = new char[12];
    sendWindowPackets(window, fd, serverAddr);
    sprintf(endChar, "end_%d", htons(RECEIVER_PORT));
    serverAddr.sin_port = htons(SENDER_PORT);

    sendto(fd, (const char *) endChar, strlen(endChar), MSG_CONFIRM, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));

    auto end = chrono::high_resolution_clock::now();
    cout << chrono::duration<double, milli>(end - begin).count() << " ms" << endl;

    close(fd);

    return 0;
}
