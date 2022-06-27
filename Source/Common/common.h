#ifndef COMMON_H
#define COMMON_H

#define SENDER_PORT 1398
#define RECEIVER_PORT 1401

#define WINDOW_SIZE 4
#define PACKET_SIZE 1500
#define EOF_DATA_SIZE 12

#define BUFFER_SIZE 10

#define SIZE PACKET_SIZE + WINDOW_SIZE * 2 + EOF_DATA_SIZE

int getNumber(char packet[]);

#endif
