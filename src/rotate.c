#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

void __swap_grayscale(pgm_point_t *a, pgm_point_t *b) {
  pgm_point_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void __swap_color(ppm_point_t *a, ppm_point_t *b) {
  ppm_point_t tmp = *a;
  *a = *b;
  *b = tmp;
}

int __get_num_transpositions(int angle) { return (4 + ((angle / 90) % 4)) % 4; }

void __transpose_square_grayscale(pgm_point_t **mat, int start_line,
                                  int start_col, int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      __swap_grayscale(&(mat[start_line + i][start_col + j]),
                       &(mat[start_line + j][start_col + i]));
    }
  }
}

void __transpose_square_color(ppm_point_t **mat, int start_line, int start_col,
                              int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      __swap_color(&(mat[start_line + i][start_col + j]),
                   &(mat[start_line + j][start_col + i]));
    }
  }
}

void __transpose_square(void **mat, int start_line, int start_col, int n,
                        int type) {
  if (type == IMAGE_TYPE_PGM) {
    __transpose_square_grayscale((pgm_point_t **)mat, start_line, start_col, n);
  } else {
    __transpose_square_color((ppm_point_t **)mat, start_line, start_col, n);
  }
}

int rotate_all(image_file_t *img_file, int angle) {
  // int num_t = __get_num_transpositions(angle);

  // void **new_mat = malloc(img_file->width * (img_file->type == IMAGE_TYPE_PGM
  //                                                ? sizeof(pgm_point_t **)
  //                                                : sizeof(ppm_point_t **)));

  // for (int i = 0; i < img_file->width; ++i) {
  //   new_mat[i] = malloc(img_file->height * (img_file->type == IMAGE_TYPE_PGM
  //                                               ? sizeof(pgm_point_t)
  //                                               : sizeof(ppm_point_t)));
  // }

  // for (int i = 0; i < img_file->height; ++i) {
  //   for (int j = 0; j < img_file->width; ++j) {
  //     if (img_file->type == IMAGE_TYPE_PGM) {
  //       ((pgm_point_t **)new_mat)[j][i] = ((pgm_point_t
  //       **)img_file->mat)[i][j];
  //     }
  //   }
  // }
}

int rotate_square(image_file_t *img_file, selection_t *sel, int angle) {
  int num_t = __get_num_transpositions(angle);

  int sel_height = sel->bot_right.line - sel->top_left.line;
  int sel_width = sel->bot_right.col - sel->top_left.col;

  if (sel_height != sel_width) {
    fprintf(stderr, "Selection must be square\n");
  }

  for (int i = 0; i < num_t; ++i) {
    __transpose_square(img_file->mat, sel->top_left.line, sel->top_left.col,
                       sel_height, img_file->type);
  }

  return 1;
}