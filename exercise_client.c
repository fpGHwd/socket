#include "connection.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  int i;
  int ret;
  int data_socket;
  char buffer[BUFFER_SIZE];

  fd_set set;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 500000;
  int rv;

  /* Create local socket. */

  data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (data_socket == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /*
   * For portability clear the whole structure, since some
   * implementations have additional (nonstandard) fields in
   * the structure.
   */

  memset(&addr, 0, sizeof(struct sockaddr_un));

  /* Connect socket to socket address */

  addr.sun_family = AF_UNIX;
  /* strncpy(addr.sun_path, SOCKET_NAME, */
  /*         sizeof(addr.sun_path) - 1); // setting addr path */
  strncpy(addr.sun_path, "/tmp/sockettest",
          sizeof(addr.sun_path) - 1); // setting addr path
  ret = connect(data_socket, (const struct sockaddr *)&addr,
                sizeof(struct sockaddr_un));
  if (ret == -1) {
    fprintf(stderr, "The server is down.\n");
    exit(EXIT_FAILURE);
  }

  // read from keyboard
  for (;;) {
    // send to server
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      perror("read from keyboard");
      continue;
    }

    ret = write(data_socket, buffer, strlen(buffer) + 1); // argv[i] from 1 on
    if (ret == -1) {
      perror("write");
      break;
    } else {
      //  success
    }

    // receive from sever with timeout
    FD_ZERO(&set);             /* clear the set */
    FD_SET(data_socket, &set); /* add our file descriptor to the set */
    rv = select(data_socket + 1, &set, NULL, NULL, &timeout);
    // printf("value of rv is %d\n", rv);
    if (rv == -1) {
      perror("select()");
    } else if (rv == 0) {
      // perror("timeout");
      // printf("No data within %lld seconds.\n", timeout.tv_sec);
      continue;

    } else { // read
      // printf("server data is available now.\n");
      ret = read(data_socket, buffer, BUFFER_SIZE);
      printf("read from server: %s\n", buffer);

      if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }

      // if receive "Q", then exit
      if (strncmp(buffer, "Q", BUFFER_SIZE) == 0) {
        printf("exiting from server for receiving 'Q'\n");
        break;
      }
    }
  } // for loop

  close(data_socket);

  exit(EXIT_SUCCESS); // success
}
