#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_utils.h"
#include "defines.h"

typedef struct pgm_point {
  char red;
  char green;
  char blue;
} pgm_point_t;

typedef struct pgm_file {
  char *filename;
  int width, height;
  pgm_point_t **mat;
} pgm_file_t;

int main() {
  char cmd_buffer[1 + CMD_BUFFER_SIZE];

  pgm_file_t current_file = {NULL, 0, 0, NULL};

  while (1) {
    printf("> ");
    fgets(cmd_buffer, CMD_BUFFER_SIZE * sizeof(char), stdin);

    // check if command length exceeded buffer size
    // ( this should never happen )
    if (cmd_buffer[strlen(cmd_buffer) - 1] != '\n') {
      fprintf(stderr,
              "[error] Command length too long, has to be <= 1000 chars\n");
      continue;
    }

    // remove trailing '\n'
    cmd_buffer[strlen(cmd_buffer) - 1] = '\0';

#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Command: '%s'\n", cmd_buffer);
#endif

    // load command
    if (check_command(cmd_buffer, "LOAD")) {
      char *filename = NULL;
      get_load_cmd_arg(cmd_buffer, &filename);

      if (!filename) {
        fprintf(stderr, "[error] No filename for 'LOAD' found\n");
        continue;
      }

#ifdef MODE_DEBUG
      fprintf(stderr, "[debug] Loading file: '%s'\n", filename);
#endif

      // FILE *file = fopen(filename, "rb");

      free(filename);
    }

    else if (check_command(cmd_buffer, "EXIT")) {
      return 0;
    }
  }

  return 0;
}