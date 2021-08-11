#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PACKET_SIZE_MAXIMUM 65535

#define PACKET_PREFIX_LEN 3
//#define PACKET_PREFIX_BYTE 0xFFFFFF
#define PACKET_POSTFIX_LEN 2
//#define PACKET_POSTFIX_BYTE 0x0D0A // \r\n
#define PACKET_LEN_OCCUPIED_SIZE 2

typedef unsigned char byte;

int read_a_packet(int *socket_fd, byte *buf, int len);
byte *wrap_a_message(unsigned char *buffer, int len);

#endif // PROTOCOL_H_
