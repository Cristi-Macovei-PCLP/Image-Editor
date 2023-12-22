#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apply_param.h"
#include "cmd_utils.h"
#include "crop.h"
#include "defines.h"
#include "equalize.h"
#include "file_utils.h"
#include "histograms.h"
#include "rotate.h"
#include "structs.h"

int main() {
  char cmd_buffer[1 + CMD_BUFFER_SIZE];

  int has_file = 0;
  image_file_t current_file = {NULL, 0, 0, 0, -1, NULL};

  int has_sel = 0;
  selection_t current_sel = {{0, 0}, {0, 0}, 0};

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
      if (has_file) {
        printf("Freeing existing file\n");
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
        has_file = 0;
        free(filename);
        continue;
      }

      current_file.filename = filename;

      has_file = 1;
      if (!parse_image_file(file, &current_file)) {
        fprintf(stderr, "Failed to load %s\n", filename);
        has_file = 0;
        free_image_file(&current_file);
      }

      fclose(file);
    }

#ifdef MODE_DEBUG
    else if (check_command(cmd_buffer, "SHOW")) {
      fprintf(stderr, "[debug] Loaded filename: %s\n", current_file.filename);
    }
#endif

    else if (check_command(cmd_buffer, "SELECT")) {
      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      selection_t new_sel;
      int is_selection_valid =
          get_select_cmd_args(cmd_buffer, &new_sel, &current_file);

      if (is_selection_valid) {
        has_sel = 1;
        current_sel = new_sel;
      } else {
        has_sel = 0;
        printf("Invalid selection\n");
      }

#ifdef MODE_DEBUG
      fprintf(stderr, "[debug] Selected %d,%d --> %d,%d\n",
              current_sel.top_left.line, current_sel.top_left.col,
              current_sel.bot_right.line, current_sel.bot_right.col);
#endif
    }

    else if (check_command(cmd_buffer, "CROP")) {
      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      if (!has_sel) {
        fprintf(stderr, "No selection set\n");
        continue;
      }

      crop_image(&current_file, &current_sel);

      printf("Cropped image\n");
    }

    else if (check_command(cmd_buffer, "ROTATE")) {
      int angle = -1;
      get_rotate_cmd_args(cmd_buffer, &angle);

#ifdef MODE_DEBUG
      fprintf(stderr, "[debug] Rotate angle = %d\n", angle);
#endif

      if (abs(angle) != 90 && abs(angle) != 180 && abs(angle) != 270 &&
          abs(angle) != 360) {
        fprintf(stderr, "Unsupported rotation angle\n");
        continue;
      }

      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      if (!has_sel) {
        fprintf(stderr, "No selection set");
        continue;
      }

      if (current_sel.is_all) {
        rotate_all(&current_file, angle);
      } else {
        rotate_square(&current_file, &current_sel, angle);
      }
    }

    else if (check_command(cmd_buffer, "HISTOGRAM")) {
      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      if (current_file.type != IMAGE_GRAYSCALE) {
        fprintf(stderr, "Black and white image needed\n");
        continue;
      }

      int x, y;
      get_histogram_cmd_args(cmd_buffer, &x, &y);

      print_histogram(&current_file, x, y);
    }

    else if (check_command(cmd_buffer, "APPLY")) {
      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      if (current_file.type == IMAGE_GRAYSCALE) {
        fprintf(stderr, "Easy, Charlie Chaplin\n");
        continue;
      }

      int apply_param_type = APPLY_PARAM_BAD;
      get_apply_cmd_args(cmd_buffer, &apply_param_type);

      if (apply_param_type == APPLY_PARAM_BAD) {
        fprintf(stderr, "APPLY parametru invalid");
        continue;
      }

      apply_param(&current_file, apply_param_type);
    }

    else if (check_command(cmd_buffer, "EQUALIZE")) {
      if (!has_file) {
        fprintf(stderr, "No image loaded\n");
        continue;
      }

      if (current_file.type != IMAGE_GRAYSCALE) {
        fprintf(stderr, "Black and white image needed\n");
        continue;
      }

      equalize_image(&current_file);

      printf("Equalization done\n");
    }

    else if (check_command(cmd_buffer, "SAVE")) {
      char *filename = NULL;
      get_save_cmd_args(cmd_buffer, &filename);

      if (!filename) {
        fprintf(stderr, "No filename given\n");
        continue;
      }

      printf("Saving to %s\n", filename);

      save_image_binary(&current_file, filename);

      free(filename);
    }

    else if (check_command(cmd_buffer, "EXIT")) {
      printf("[debug] Exiting program with %sloaded file\n",
             has_file ? "1 " : "no ");
      if (has_file) {
        free_image_file(&current_file);
        // printf("Freeing image file name %p\n", current_file.filename);
        // free(current_file.filename);

        // for (int i = 0; i < current_file.height; ++i) {
        //   printf("Freeing image row: %p\n", current_file.mat[i]);
        //   free(current_file.mat[i]);
        // }
        // printf("Freeing image matrix %p\n", current_file.mat);
        // free(current_file.mat);
      }
      return 0;
    } else {
      printf("Unknown command\n");
      continue;
    }
  }

  return 0;
}