#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

// ! vezi ca n ai testat asta
int crop_image(image_file_t *img_file, selection_t *sel) {
  int new_width = sel->bot_right.col - sel->top_left.col;
  int new_height = sel->bot_right.line - sel->top_left.line;

  int old_height = img_file->height;

  img_file->width = new_width;
  img_file->height = new_height;

  for (int i = 0; i < new_height; ++i) {
    for (int j = 0; j < new_width; ++j) {
      int line = sel->top_left.line + i;
      int col = sel->top_left.col + j;

      if (img_file->type == IMAGE_GRAYSCALE) {
        ((pgm_point_t **)img_file->mat)[i][j].grey =
            ((pgm_point_t **)img_file->mat)[line][col].grey;
      } else {
        ((ppm_point_t **)img_file->mat)[i][j].red =
            ((ppm_point_t **)img_file->mat)[line][col].red;

        ((ppm_point_t **)img_file->mat)[i][j].green =
            ((ppm_point_t **)img_file->mat)[line][col].green;

        ((ppm_point_t **)img_file->mat)[i][j].blue =
            ((ppm_point_t **)img_file->mat)[line][col].blue;
      }
    }
  }

  // free memory
  for (int i = 0; i < new_height; ++i) {
    void *new_line = realloc(img_file->mat[i], new_width * sizeof(void *));
    if (!new_line) {
      fprintf(stderr, "A plesnit realloc-u :(\n");
      exit(-1);
    }

    img_file->mat[i] = new_line;
  }
  for (int i = new_height; i < old_height; ++i) {
    free(img_file->mat[i]);
  }

  void **new_mat = realloc(img_file->mat, new_height * sizeof(void **));
  if (!new_mat) {
    fprintf(stderr, "A plesnit realloc-u :(\n");
    exit(-1);
  }
  img_file->mat = new_mat;

  return 1;
}