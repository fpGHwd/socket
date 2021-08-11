#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <time.h>

#define PACKET_START_BYTE 0xFFFFFF
#define PACKET_SIZE_MAXIMUM 65535
#define PACKET_END_BYTE 0x0D0A // \r\n
#define PACKET_PREFIX_N_LEN_SIZE 5

typedef unsigned char byte;

int read_a_packet(int *socket_fd, byte *buf, int len);

#endif // PROTOCOL_H_
