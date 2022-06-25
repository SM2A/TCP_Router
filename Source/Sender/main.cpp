// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <chrono>

using namespace std;

#define PORT 8000
#define PACKET_SIZE 1500
#define WINDOW_SIZE 4
#define RECIEVER_PORT 8080

void sendWidnowPackets(char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12], const int sock,
                       struct sockaddr_in &addr, const int last_packet_num) {
    char message[WINDOW_SIZE * 4];
    message[0] = '\0';
    socklen_t len;
    int n;
    while (message[0] != 'A') {
        for (int k = 0; k < WINDOW_SIZE; k++) {
            addr.sin_port = htons(PORT);
            sendto(sock, (const char *) window[k], strlen(window[k]),
                   MSG_CONFIRM, (const struct sockaddr *) &addr,
                   sizeof(addr));
        }
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        if ((n = recvfrom(sock, (char *) message, PACKET_SIZE,
                          MSG_WAITALL, (struct sockaddr *) &addr,
                          &len)) < 0) {
            message[0] = '\0';
            continue;
        }
        message[n] = '\0';
    }
    for (int k = 0; k < WINDOW_SIZE; k++) {
        memset(window[k], 0, PACKET_SIZE + WINDOW_SIZE * 2 + 12);
    }
}

void addPacketNum(char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12], int j, int &packet_num) {
    char *ch = new char[12];
    sprintf(ch, "_%d_%d", packet_num, htons(RECIEVER_PORT));
    strncat(window[j], ch, sizeof(ch));
    packet_num = (packet_num + 1) % (WINDOW_SIZE * 2);
}

int main() {
    int sock;
    char character;
    char packet[PACKET_SIZE + 1];
    int i = 0, j = 0, packet_num = 0;
    char window[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12] = {0};
    struct sockaddr_in servaddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    fstream fin("Data/Data.txt", fstream::in);
    auto start_time = chrono::high_resolution_clock::now();
    while (fin >> noskipws >> character) {
        window[j][i++] = character;
        if (i == 10) {
            addPacketNum(window, j, packet_num);
            i = 0;
            j++;
        }
        if (j == WINDOW_SIZE) {
            sendWidnowPackets(window, sock, servaddr, packet_num);
            j = 0;
        }
    }
    addPacketNum(window, j, packet_num);
    while (j < WINDOW_SIZE) {
        i = 0;
        j++;
        addPacketNum(window, j, packet_num);
    }
    sendWidnowPackets(window, sock, servaddr, packet_num);
    char *endChar = new char[12];
    sprintf(endChar, "end_%d", htons(RECIEVER_PORT));
    servaddr.sin_port = htons(PORT);
    sendto(sock, (const char *) endChar, strlen(endChar),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));
    close(sock);
    auto end_time = chrono::high_resolution_clock::now();
    double duration_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    cout << "Duration time is: " << duration_time << endl;
    return 0;
}
