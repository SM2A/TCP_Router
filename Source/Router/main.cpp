#include <iostream>

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <queue>

#define ADDRESS "192.168.1.255"
#define BUFFER 1024

using namespace std;

queue<string> data;

int receivePort;
int sendPort;

void receivePacket(int port);
void sendPacket(int port);

void *rp(void *) {
    receivePacket(receivePort);
    return nullptr;
}

void *sp(void *) {
    sendPacket(sendPort);
    return nullptr;
}

int main() {

    pthread_t threads[2];
    pthread_create(&threads[0], nullptr, &rp, nullptr);
    pthread_create(&threads[1], nullptr, &sp, nullptr);
    for (unsigned long thread : threads) pthread_join(thread, nullptr);

    return 0;
}

void receivePacket(int port) {

    int broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr(ADDRESS);

    bind(sock, (struct sockaddr *) &bc_address, sizeof(bc_address));

    while (true) {
        char temp[BUFFER] = {0};
        recv(sock, temp, BUFFER, 0);
        data.push(string(temp));
    }
}

void sendPacket(int port) {

    int broadcast = 1, opt = 1;
    struct sockaddr_in bc_address;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr(ADDRESS);

    bind(sock, (struct sockaddr *) &bc_address, sizeof(bc_address));

    while (true) {
        if (!data.empty()) {
            sendto(sock, data.front().c_str(), strlen(data.front().c_str()), 0, (struct sockaddr *) &bc_address, sizeof(bc_address));
            data.pop();
        }
    }
}
