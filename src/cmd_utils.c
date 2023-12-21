#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "structs.h"

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

  if (isalnum(cmd_buffer[strlen(cmd_name)])) {
    return 0;
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

// todo make this return int
void get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;
  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] select argument %d = '%s'\n", arg_index, p);
#endif
    if (arg_index == 1) {
      if (strcmp(p, "ALL") == 0) {
        ptr_sel->is_all = 1;
        return;
      }

      ptr_sel->top_left.line = atoi(p);
    } else if (arg_index == 2) {
      ptr_sel->top_left.col = atoi(p);
    } else if (arg_index == 3) {
      ptr_sel->bot_right.line = atoi(p);
    } else if (arg_index == 4) {
      ptr_sel->bot_right.col = atoi(p);
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
  ptr_sel->is_all = 0;
}

// todo this also needs to return int
void get_histogram_cmd_args(char *cmd_buffer, int *ptr_x, int *ptr_y) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;
  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

    // printf("p = '%s'\n", p);

    if (arg_index == 1) {
      *ptr_x = atoi(p);
    } else if (arg_index == 2) {
      *ptr_y = atoi(p);
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
}

void get_save_cmd_args(char *cmd_buffer, char **ptr_filename) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;

  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

    if (arg_index == 1) {
      char *filename = malloc((1 + strlen(p)) * sizeof(char));
      strcpy(filename, p);
      *ptr_filename = filename;
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
}

void get_apply_cmd_args(char *cmd_buffer, int *ptr_arg) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;

  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

    if (arg_index == 1) {
      if (strcmp(p, "EDGE") == 0) {
        *ptr_arg = APPLY_PARAM_EDGE;
      } else if (strcmp(p, "SHARPEN")) {
        *ptr_arg = APPLY_PARAM_SHARPEN;
      } else if (strcmp(p, "BLUR")) {
        *ptr_arg = APPLY_PARAM_BLUR;
      } else if (strcmp(p, "GAUSSIAN_BLUR")) {
        *ptr_arg = APPLY_PARAM_GAUSSIAN_BLUR;
      } else {
        *ptr_arg = APPLY_PARAM_BAD;
      }
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
}

void get_rotate_cmd_args(char *cmd_buffer, int *ptr_angle) {
  char *p = strtok(cmd_buffer, " ");
  int arg_index = 0;

  while (p) {
    if (strlen(p) == 0) {
      p = strtok(NULL, " ");
      continue;
    }

    if (arg_index == 1) {
      *ptr_angle = atoi(p);
    }

    ++arg_index;
    p = strtok(NULL, " ");
  }
}
