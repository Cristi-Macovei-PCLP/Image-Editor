#include <stdio.h>

#include "defines.h"

int main() {
  char cmd_buffer[CMD_BUFFER_SIZE];

  while (1) {
    printf("> ");
    fgets(cmd_buffer, CMD_BUFFER_SIZE * sizeof(char), stdin);

#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Command: %s\n", cmd_buffer);
#endif
  }

  return 0;
}