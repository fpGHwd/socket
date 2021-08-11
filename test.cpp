//#include "test.h"
#include "protocol.h"
#include <cstdlib>
#include <gtest/gtest.h>

using namespace std;

int test_wrapped_message() {
  char *ts = "hello";
  unsigned char result[] = {0xff, 0xff, 0xff, 0x68, 0x65,
                            0x6c, 0x6c, 0x6f, 0x0d, 0x0a};
  unsigned char *wrapped_message = wrap_a_message((unsigned char *)ts, 5);

  for (int i = 0; i < 10; i++) {
    if (*(result + i) != *(wrapped_message + i)) {
      return -1;
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  test_wrapped_message();
  return 0;
}
