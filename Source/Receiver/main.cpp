#include <cstring>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

#define SENDER_PORT 1398
#define RECEIVER_PORT 1401

#define WINDOW_SIZE 4
#define PACKET_SIZE 1500

int getSize(char *s) {
    char *t;
    for (t = s; *t != '\0'; t++);
    return t - s;
}

int getPacketNum(char *packet) {
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

void sendAck(int &expected_packet_num, const int sock, struct sockaddr_in &addr, const int dest_port) {
    char *ack_message = new char[20];
    sprintf(ack_message, "ACK-%d_%d", expected_packet_num, dest_port);
    addr.sin_port = htons(SENDER_PORT);
    sendto(sock, (const char *) ack_message, strlen(ack_message),MSG_CONFIRM, (const struct sockaddr *) &addr,sizeof(addr));
}

int main() {

    bool receivedAll, receivedPackets[WINDOW_SIZE];
    int port, packet_num, expected_packet_num = 0;
    char message[WINDOW_SIZE][PACKET_SIZE + WINDOW_SIZE * 2 + 12];

    struct sockaddr_in servAddr, clientAddr;

    memset(&servAddr, 0, sizeof(servAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(RECEIVER_PORT);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(fd, (const struct sockaddr *) &servAddr, sizeof(servAddr));

    socklen_t len = sizeof(clientAddr);

    ofstream file("out.txt");

    memset(receivedPackets, false, WINDOW_SIZE);

    while (true) {

        char packet[PACKET_SIZE + WINDOW_SIZE * 2 + 12];
        int n = recvfrom(fd, (char *) packet, PACKET_SIZE + WINDOW_SIZE * 2 + 12, MSG_WAITALL,
                     (struct sockaddr *) &clientAddr, &len);
        packet[n] = '\0';
        port = getDestPort(packet);
        if (!strcmp(packet, "end")) {
            break;
        }
        packet_num = getPacketNum(packet);
        if (expected_packet_num <= packet_num && expected_packet_num + WINDOW_SIZE > packet_num) {
            strncpy(message[packet_num % WINDOW_SIZE], packet, PACKET_SIZE + WINDOW_SIZE * 2 + 12);
            receivedPackets[packet_num % WINDOW_SIZE] = true;
        } else {
            sendAck(expected_packet_num, fd, clientAddr, port);
        }

        receivedAll = true;
        for (bool receivedPacket : receivedPackets) if (!receivedPacket) receivedAll = false;

        if (receivedAll) {
            memset(receivedPackets, false, WINDOW_SIZE);
            for (auto & i : message) file << i;
            expected_packet_num = (expected_packet_num + WINDOW_SIZE) % (2 * WINDOW_SIZE);
            sendAck(expected_packet_num, fd, clientAddr, port);
        }
    }

    file.close();

    return 0;
}
