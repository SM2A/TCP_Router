#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

using namespace std;

#define PORT 8080
#define PACKET_SIZE 1500
#define WINDOW_SIZE 4
#define SENDER_PORT 3000

int getSize(char *s) {
    char *t;
    for (t = s; *t != '\0'; t++);
    return t - s;
}

int getPacketNum(char packet[PACKET_SIZE + WINDOW_SIZE * 2 + 12]) {
    int num = 0, j = 1;
    for (int i = getSize(packet) - 1; i >= 0; i--) {
        if (packet[i] == '_') {
            packet[i] = '\0';
            return num;
        }
        num += (int) (packet[i] - '0') * j;
        j *= 10;
    }
    return -1;
}

int getDestPort(char message[PACKET_SIZE + WINDOW_SIZE * 2 + 12]) {
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

void addDestPort(char *message, const int port) {
    char *ch = new char[12];
    sprintf(ch, "_%d", port);
    strncat(message, ch, sizeof(ch));
}

void sendAck(int &expected_packet_num, const int sock, struct sockaddr_in &addr, const int dest_port) {
    char *ack_message = new char[20];
    sprintf(ack_message, "ACK-%d_%d", expected_packet_num, dest_port);
    addr.sin_port = htons(8000);
    sendto(sock, (const char *) ack_message, strlen(ack_message),
           MSG_CONFIRM, (const struct sockaddr *) &addr,
           sizeof(addr));
}

int main() {
    int sock, packet_num, expected_packet_num = 0, port;
    char message[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12];
    bool recieved_packets[WINDOW_SIZE];
    bool recieved_all;

    struct sockaddr_in servaddr, cliaddr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sock, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n;

    socklen_t len = sizeof(cliaddr);

    ofstream MyFile("out.txt");
    memset(recieved_packets, false, WINDOW_SIZE);

    while (true) {

        char packet[PACKET_SIZE + WINDOW_SIZE * 2 + 12];
        n = recvfrom(sock, (char *) packet, PACKET_SIZE + WINDOW_SIZE * 2 + 12, MSG_WAITALL,
                     (struct sockaddr *) &cliaddr, &len);
        packet[n] = '\0';
        port = getDestPort(packet);
        if (!strcmp(packet, "end")) {
            break;
        }
        packet_num = getPacketNum(packet);
        if (expected_packet_num <= packet_num && expected_packet_num + WINDOW_SIZE > packet_num) {
            strncpy(message[packet_num % WINDOW_SIZE], packet, PACKET_SIZE + WINDOW_SIZE * 2 + 12);
            recieved_packets[packet_num % WINDOW_SIZE] = true;
        } else {
            sendAck(expected_packet_num, sock, cliaddr, port);
        }

        recieved_all = true;
        for (int i = 0; i < WINDOW_SIZE; i++) {
            if (!recieved_packets[i]) {
                recieved_all = false;
            }
        }
        if (recieved_all) {
            memset(recieved_packets, false, WINDOW_SIZE);
            for (int i = 0; i < WINDOW_SIZE; i++) {
                MyFile << message[i];
            }
            expected_packet_num = (expected_packet_num + WINDOW_SIZE) % (2 * WINDOW_SIZE);
            sendAck(expected_packet_num, sock, cliaddr, port);
        }
    }
    MyFile.close();
    return 0;
}
