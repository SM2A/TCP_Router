#ifndef PACKET_H
#define PACKET_H

struct Packet {

    int type;
    int seqnum;
    int acknum;
    int length;
    char data[1450];
};

#endif
