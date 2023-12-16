#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_utils.h"
#include "defines.h"
#include "file_utils.h"
#include "structs.h"

int main() {
  char cmd_buffer[1 + CMD_BUFFER_SIZE];

  image_file_t current_file = {NULL, 0, 0, 0, -1, NULL};

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
      // free loaded file if exists
      if (current_file.filename != NULL) {
        free_image_file(&current_file);
      }

      char *filename = NULL;
      get_load_cmd_arg(cmd_buffer, &filename);

      if (!filename) {
        fprintf(stderr, "[error] No filename for 'LOAD' found\n");
        continue;
      }

#ifdef MODE_DEBUG
      fprintf(stderr, "[debug] Loading file: '%s'\n", filename);
#endif

      FILE *file = fopen(filename, "rb");

      if (!file) {
        fprintf(stderr, "Failed to load %s\n", filename);
        free(filename);
        continue;
      }

      current_file.filename = filename;

      if (!parse_image_file(file, &current_file)) {
        fprintf(stderr, "Failed to load %s\n", filename);
        free_image_file(&current_file);
      }

      fclose(file);
    }

#ifdef MODE_DEBUG
    else if (check_command(cmd_buffer, "SHOW")) {
      fprintf(stderr, "[debug] Loaded filename: %s\n", current_file.filename);
    }
#endif
    else if (check_command(cmd_buffer, "EXIT")) {
      if (current_file.filename != NULL) {
        free_image_file(&current_file);
      }
      return 0;
    }
  }

  return 0;
}