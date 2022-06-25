#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <queue>

using namespace std;

#define PORT 8000
#define RECIEVER_PORT 8080
#define SENDER_PORT 3000
#define PACKET_SIZE 1500
#define BUFFER_SIZE 10
#define WINDOW_SIZE 4

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
    int sockfd, reciever_sock;
    char message[PACKET_SIZE + WINDOW_SIZE * 2 + 12];
    char *to_send_message;
    queue<char *> buffer;
    struct sockaddr_in servaddr, cliaddr, reciever_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    if ((reciever_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&reciever_addr, 0, sizeof(reciever_addr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    reciever_addr.sin_family = AF_INET;
    reciever_addr.sin_addr.s_addr = INADDR_ANY;
    reciever_addr.sin_port = htons(RECIEVER_PORT);

    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n;

    socklen_t len = sizeof(cliaddr);

    while ((n = recvfrom(sockfd, (char *) message, PACKET_SIZE + WINDOW_SIZE * 2 + 12, MSG_WAITALL,
                         (struct sockaddr *) &cliaddr, &len)) != -1) {
        message[n] = '\0';
        if (buffer.size() < BUFFER_SIZE) {
            buffer.push(message);
        }

        if (!buffer.empty()) {
            to_send_message = buffer.front();
            buffer.pop();
            int port = getDestPort(to_send_message);
            reciever_addr.sin_port = port;
            addSourcePort(to_send_message, cliaddr.sin_port);
            if (rand() % 10 != 0) {
                sendto(reciever_sock, (const char *) to_send_message, strlen(to_send_message),
                       MSG_CONFIRM, (const struct sockaddr *) &reciever_addr,
                       sizeof(reciever_addr));
            }
        }
    }

    return 0;
}
