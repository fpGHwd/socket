#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

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

  while (read_len < PACKET_PREFIX_N_LEN_SIZE || read_len < packet_len) {

    to_read_len =
        (read_len < PACKET_PREFIX_N_LEN_SIZE ? PACKET_PREFIX_N_LEN_SIZE
                                             : (packet_len - read_len));

    rv = select(*socket_fd + 1, &set, NULL, NULL, &timeout);
    // if client release, should detect this case here

    if (rv == -1) {
      perror("select()");
      break;
    } else if (rv == 0) {
      // timeout
    } else {
      ret = read(*socket_fd, buffer, to_read_len);
      if (packet_len == -1 && read_len >= PACKET_PREFIX_N_LEN_SIZE) {
        // check first three bytes
        if (!(buffer[0] == 0xFF && buffer[1] == 0xFF && buffer[2] == 0xFF)) {
          return -2; // protocol error
        }
        // get package_len from packet prefix
        packet_len = ((buffer[3] << 4) + buffer[4]);
      }
      read_len += ret;
    }
  }

  if (!(buffer[read_len - 2] != 0x0D && buffer[read_len - 1] != 0x0A)) {
    return -2; // protocol error
  }

  memcpy(buf, buffer, read_len);

  return read_len;
}
