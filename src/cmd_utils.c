#include <stdlib.h>
#include <string.h>

int check_command(char *cmd_buffer, char *cmd_name) {
  if (strlen(cmd_buffer) < strlen(cmd_name)) {
    return 0;
  }

  // i know strcmp is a thing, i just don't like it
  for (int i = 0; i < (int)strlen(cmd_name); ++i) {
    if (cmd_buffer[i] != cmd_name[i]) {
      return 0;
    }
  }

  return 1;
}

void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;
  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

    if (arg_index == 1) {
      char *filename = malloc(1 + strlen(p) * sizeof(char));
      strcpy(filename, p);

      *ptr_filename = filename;
      return;
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
}