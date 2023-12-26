#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

/*
1 2 3 4                 4 8 2 6
5 6 7 8                 3 7 1 5
9 0 1 2      <-----     2 6 0 4
3 4 5 6                 1 5 9 3

   |                       ^
   |                       |
   V                       |

3 9 5 1                 6 5 4 3
4 0 6 2                 2 1 0 9
5 1 7 3      ----->     8 7 6 5
6 2 8 4                 4 3 2 1
*/

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

int __get_num_rotations(int angle) { return (4 + ((angle / 90) % 4)) % 4; }

void __rotate_square_90_grayscale(pgm_point_t **mat, int start_line,
                                  int start_col, int n) {
  pgm_point_t **aux = malloc(n * sizeof(pgm_point_t *));
  for (int i = 0; i < n; ++i) {
    aux[i] = malloc(n * sizeof(pgm_point_t));
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      aux[i][j] = mat[start_line + i][start_col + j];
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      mat[start_line + i][start_col + j] = aux[n - j - 1][i];
    }
  }

  for (int i = 0; i < n; ++i) {
    free(aux[i]);
  }
  free(aux);
}

void __rotate_square_90_color(ppm_point_t **mat, int start_line, int start_col,
                              int n) {
  ppm_point_t **aux = malloc(n * sizeof(ppm_point_t *));
  for (int i = 0; i < n; ++i) {
    aux[i] = malloc(n * sizeof(ppm_point_t));
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      aux[i][j] = mat[start_line + i][start_col + j];
    }
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      mat[start_line + i][start_col + j] = aux[n - j - 1][i];
    }
  }

  for (int i = 0; i < n; ++i) {
    free(aux[i]);
  }
  free(aux);
}

void __rotate_square_90(void **mat, int start_line, int start_col, int n,
                        int type) {
  if (type == IMAGE_GRAYSCALE) {
    __rotate_square_90_grayscale((pgm_point_t **)mat, start_line, start_col, n);
  } else if (type == IMAGE_COLOR) {
    __rotate_square_90_color((ppm_point_t **)mat, start_line, start_col, n);
  }
}

pgm_point_t **__rotate_all_90_grayscale(pgm_point_t **mat, int n, int m) {
  pgm_point_t **aux = malloc(m * sizeof(pgm_point_t *));
  for (int i = 0; i < m; ++i) {
    aux[i] = malloc(n * sizeof(pgm_point_t));
  }

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      aux[i][j] = mat[n - j - 1][i];
    }
  }

  for (int i = 0; i < n; ++i) {
    free(mat[i]);
  }
  free(mat);

  return aux;
}

ppm_point_t **__rotate_all_90_color(ppm_point_t **mat, int n, int m) {
  ppm_point_t **aux = malloc(m * sizeof(ppm_point_t *));
  for (int i = 0; i < m; ++i) {
    aux[i] = malloc(n * sizeof(ppm_point_t));
  }

  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      aux[i][j] = mat[n - j - 1][i];
    }
  }

  for (int i = 0; i < n; ++i) {
    free(mat[i]);
  }
  free(mat);

  return aux;
}

void __rotate_all_90(image_file_t *img_file, int type) {
  if (type == IMAGE_COLOR) {
    img_file->mat = (void **)__rotate_all_90_color(
        (ppm_point_t **)img_file->mat, img_file->height, img_file->width);
  } else {
    img_file->mat = (void **)__rotate_all_90_grayscale(
        (pgm_point_t **)img_file->mat, img_file->height, img_file->width);
  }

  int tmp = img_file->height;
  img_file->height = img_file->width;
  img_file->width = tmp;
}

int rotate_square(image_file_t *img_file, selection_t *sel, int angle) {
  int num_t = __get_num_rotations(angle);

  int sel_height = sel->bot_right.line - sel->top_left.line;
  int sel_width = sel->bot_right.col - sel->top_left.col;

  if (sel_height != sel_width) {
    fprintf(stderr, "Selection must be square\n");
    return 1;
  }

  for (int i = 0; i < num_t; ++i) {
    __rotate_square_90(img_file->mat, sel->top_left.line, sel->top_left.col,
                       sel_height, img_file->type);
  }

  return 1;
}

int rotate_all(image_file_t *img_file, selection_t *sel, int angle) {
  int num_t = __get_num_rotations(angle);

  for (int i = 0; i < num_t; ++i) {
    __rotate_all_90(img_file, img_file->type);
  }

  // flip selection if number of 90 deg rotations is odd ( 1 or 3 )
  if (num_t % 2 == 1) {
    int tmp = sel->bot_right.line;
    sel->bot_right.line = sel->bot_right.col;
    sel->bot_right.col = tmp;
  }

  return 1;
}