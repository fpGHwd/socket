#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

byte packet_prefix_byte[] = {0xFF, 0xFF, 0xFF};
byte packet_postfix_byte[] = {0x0D, 0x0A};

/*
len: buff size
  */
int read_a_packet(int *socket_fd, byte *buf, int len) {
  fd_set set;
  int rv;
  int ret;
  int read_len = 0;
  int packet_len = -1;
  struct timeval timeout;
  byte buffer[len];
  int to_read_len;

  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;

  FD_ZERO(&set);
  FD_SET(*socket_fd, &set);

  while (read_len < PACKET_PREFIX_LEN || read_len < packet_len) {

    to_read_len = (read_len < (PACKET_PREFIX_LEN + PACKET_LEN_OCCUPIED_SIZE)
                       ? (PACKET_PREFIX_LEN + PACKET_LEN_OCCUPIED_SIZE)
                       : (packet_len - read_len));
    // rv = select(*socket_fd + 1, &set, NULL, NULL, &timeout);
    rv = 1;
    // if client release, should detect this case here

    if (rv == -1) {
      perror("select()");
      break;
    } else if (rv == 0) {
      // timeout
    } else {
      ret = read(*socket_fd, buffer + read_len, to_read_len);
      read_len += ret;
      if (packet_len == -1 && read_len >= PACKET_PREFIX_LEN) {
        // check first three bytes
        if (!(buffer[0] == 0xFF && buffer[1] == 0xFF && buffer[2] == 0xFF)) {
          return -2; // protocol error
        }
        // get package_len from packet prefix
        packet_len = ((buffer[3] << 4) + buffer[4]);
      }
    }
  }

  if (buffer[read_len - 2] != 0x0D && buffer[read_len - 1] != 0x0A) {
    return -2; // protocol error
  }

  // printf("read length %d from client:\n");

  memcpy(buf, buffer, read_len);

  return read_len;
}

unsigned char *wrap_a_message(unsigned char *buffer, int len) {
  int p = 0;
  int all_len = 0;
  all_len =
      len + PACKET_PREFIX_LEN + PACKET_LEN_OCCUPIED_SIZE + PACKET_POSTFIX_LEN;

  byte *wrapped_message = (byte *)malloc(sizeof(unsigned char) * all_len);
  if (wrapped_message == NULL) {
    return NULL;
  }

  memcpy(wrapped_message + p, packet_prefix_byte, PACKET_PREFIX_LEN);
  p += PACKET_PREFIX_LEN;

  // len of message body
  wrapped_message[p++] = all_len / 0x10;
  wrapped_message[p++] = all_len % 0x10;

  memcpy(wrapped_message + p, buffer, len);
  p += len;

  memcpy(wrapped_message + p, packet_postfix_byte, PACKET_POSTFIX_LEN);

  return wrapped_message;
}
