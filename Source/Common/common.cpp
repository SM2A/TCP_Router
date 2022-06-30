#include <cstring>

int getNumber(char packet[]) {
    int num = 0, mult = 1;
    for (int i = strlen(packet) - 1; i >= 0; i--) {
        if (packet[i] == '#') {
            packet[i] = '\0';
            return num;
        }
        num += (int) (packet[i] - '0') * mult;
        mult *= 10;
    }
    return -1;
}
