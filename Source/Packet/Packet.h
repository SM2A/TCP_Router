#ifndef PACKET_H
#define PACKET_H
 
struct Packet {
	
    int type;           // 0 if an ACK, 1 if a data packet
 	int seqnum;         // sequence number
    int acknum;         // acknowledge number
	int length;         // number of characters carried in data field 
	char data[1450];    // remember this should be 0 for ACK packets
};
 
#endif