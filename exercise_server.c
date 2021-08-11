#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

void delay_m(int milliseconds) {
  long pause;
  clock_t now, then;

  pause = milliseconds * (CLOCKS_PER_SEC / 1000);
  now = then = clock();
  while ((now - then) < pause)
    now = clock();
}

int main(int argc, char *argv[]) {
  struct sockaddr_un name;
  int down_flag = 0;
  int down_flag_5 = 0;
  int ret;
  int connection_socket;
  int data_socket;
  int result;
  char buffer[BUFFER_SIZE];

  /*
   * In case the program exited inadvertently on the last run,
   * remove the socket.
   */

  unlink(SOCKET_NAME);

  /* Create local socket. */

  connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (connection_socket == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /*
   * For portability clear the whole structure, since some
   * implementations have additional (nonstandard) fields in
   * the structure.
   */
  memset(&name, 0, sizeof(struct sockaddr_un));

  /* Bind socket to socket name. */

  name.sun_family = AF_UNIX;
  // strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
  if (argc == 1) {
    strncpy(name.sun_path, "/tmp/sockettest", sizeof(name.sun_path) - 1);
  } else {
    strncpy(name.sun_path, argv[1], sizeof(name.sun_path) - 1);
  }
  // printf("sun_path is %s\n", argv[1]);

  ret = bind(connection_socket, (const struct sockaddr *)&name,
             sizeof(struct sockaddr_un));
  if (ret == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /*
   * Prepare for accepting connections. The backlog size is set
   * to 20. So while one request is being processed other requests
   * can be waiting.
   */

  ret = listen(connection_socket, 20);
  if (ret == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* This is the main loop for handling connections. */

  for (;;) {
    down_flag_5 = 0;

    /* Wait for incoming connection. */

    data_socket = accept(connection_socket, NULL, NULL);
    if (data_socket == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    result = 0;
    for (;;) {

      /* Wait for next data packet. */
      ret = read(data_socket, buffer, BUFFER_SIZE);
      if (ret == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }

      /* Ensure buffer is 0-terminated. */

      buffer[BUFFER_SIZE - 1] = 0;

      /* Handle commands. */

      if (!strncmp(buffer, "DOWN", BUFFER_SIZE)) {
        down_flag = 1; // exit after command "DOWN"
        break;
      }

      if (!strncmp(buffer, "END", BUFFER_SIZE)) {
        break; // not exit after command "END"
      }

      printf("Reading from client: %s", buffer);
      down_flag_5++;

      if (down_flag_5 >= 5) {
        sprintf(buffer, "%c", 'Q');
        // printf("buffer is %s\n", buffer);  // test buffer
        ret = write(data_socket, buffer, BUFFER_SIZE);
        /* printf("delaying...\n"); */
        delay_m(5000);
        printf("delaying for 5 seconds then exit from client...\n");
        break;
      }
    }

    if (ret == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    /* Close socket. */
    close(data_socket);
    /*   if (down_flag_5 >= 5) { */
    /*     break; */
    /*   } */
  }

  close(connection_socket);

  /* Unlink the socket. */

  unlink(SOCKET_NAME);

  exit(EXIT_SUCCESS);
}
