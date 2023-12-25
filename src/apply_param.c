#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "math_utils.h"

#include "defines.h"
#include "structs.h"

double **__get_apply_mat(int param_type) {
  double **mat = malloc(3 * sizeof(double));
  for (int i = 0; i < 3; ++i) {
    mat[i] = malloc(3 * sizeof(double));
  }

  if (param_type == APPLY_PARAM_EDGE) {
    mat[0][0] = -1;
    mat[0][1] = -1;
    mat[0][2] = -1;
    mat[1][0] = -1;
    mat[1][1] = 8;
    mat[1][2] = -1;
    mat[2][0] = -1;
    mat[2][1] = -1;
    mat[2][2] = -1;
  } else if (param_type == APPLY_PARAM_SHARPEN) {
    mat[0][0] = 0;
    mat[0][1] = -1;
    mat[0][2] = 0;
    mat[1][0] = -1;
    mat[1][1] = 5;
    mat[1][2] = -1;
    mat[2][0] = 0;
    mat[2][1] = -1;
    mat[2][2] = 0;
  } else if (param_type == APPLY_PARAM_BLUR) {
    mat[0][0] = (double)1 / 9;
    mat[0][1] = (double)1 / 9;
    mat[0][2] = (double)1 / 9;
    mat[1][0] = (double)1 / 9;
    mat[1][1] = (double)1 / 9;
    mat[1][2] = (double)1 / 9;
    mat[2][0] = (double)1 / 9;
    mat[2][1] = (double)1 / 9;
    mat[2][2] = (double)1 / 9;
  } else if (param_type == APPLY_PARAM_GAUSSIAN_BLUR) {
    mat[0][0] = (double)1 / 16;
    mat[0][1] = (double)2 / 16;
    mat[0][2] = (double)1 / 16;
    mat[1][0] = (double)2 / 16;
    mat[1][1] = (double)4 / 16;
    mat[1][2] = (double)2 / 16;
    mat[2][0] = (double)1 / 16;
    mat[2][1] = (double)2 / 16;
    mat[2][2] = (double)1 / 16;
  }

  return mat;
}

int apply_param(image_file_t *img_file, selection_t *sel, int param_type) {
#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Selection %d %d --> %d %d\n", sel->top_left.line,
          sel->top_left.col, sel->bot_right.line, sel->bot_right.col);
#endif

  double **mat = __get_apply_mat(param_type);
  int d9i[] = {-1, -1, 0, 1, 1, 1, 0, -1, 0};
  int d9j[] = {0, 1, 1, 1, 0, -1, -1, -1, 0};

  int sel_height = sel->bot_right.line - sel->top_left.line;
  int sel_width = sel->bot_right.col - sel->top_left.col;

  ppm_point_t **copy = malloc(sel_height * sizeof(ppm_point_t *));
  for (int i = 0; i < sel_height; ++i) {
    copy[i] = malloc(sel_width * sizeof(ppm_point_t));
  }

  for (int i = sel->top_left.line; i < sel->bot_right.line; ++i) {
    for (int j = sel->top_left.col; j < sel->bot_right.col; ++j) {
      double new_red = 0;
      double new_green = 0;
      double new_blue = 0;

#ifdef MODE_DEBUG
      fprintf(
          stderr,
          "[debug] For point %d %d of sel (%d %d of matrix), neighbours are:\n",
          i - sel->top_left.line, j - sel->top_left.col, i, j);
#endif

      for (int dir = 0; dir < 9; ++dir) {
        int di = d9i[dir];
        int dj = d9j[dir];

        int new_i = i + di;
        int new_j = j + dj;

        if (0 <= new_i && new_i < img_file->height && 0 <= new_j &&
            new_j < img_file->width) {
#ifdef MODE_DEBUG
          fprintf(stderr,
                  "[debug] Neighbour #%d: (%d,%d) of mat, value = %d, matrix = "
                  "%lf, total = %lf\n",
                  dir, new_i, new_j,
                  ((ppm_point_t **)img_file->mat)[new_i][new_j].red,
                  mat[1 + di][1 + dj],
                  ((ppm_point_t **)img_file->mat)[new_i][new_j].red *
                      mat[1 + di][1 + dj]);
#endif
          new_red += ((ppm_point_t **)img_file->mat)[new_i][new_j].red *
                     mat[1 + di][1 + dj];
          new_green += ((ppm_point_t **)img_file->mat)[new_i][new_j].green *
                       mat[1 + di][1 + dj];
          new_blue += ((ppm_point_t **)img_file->mat)[new_i][new_j].blue *
                      mat[1 + di][1 + dj];
        } else {
#ifdef MODE_DEBUG
          fprintf(stderr, "broke\n");
#endif
          new_red = ((ppm_point_t **)img_file->mat)[i][j].red;
          new_green = ((ppm_point_t **)img_file->mat)[i][j].green;
          new_blue = ((ppm_point_t **)img_file->mat)[i][j].blue;
          break;
        }
      }

#ifdef MODE_DEBUG
      fprintf(stderr, "value at the end: %lf (clamped = %d)\n", new_red,
              clamp(round(new_red), 0, 255));
#endif

      copy[i - sel->top_left.line][j - sel->top_left.col].red =
          clamp(round(new_red), 0, 255);
      copy[i - sel->top_left.line][j - sel->top_left.col].green =
          clamp(round(new_green), 0, 255);
      copy[i - sel->top_left.line][j - sel->top_left.col].blue =
          clamp(round(new_blue), 0, 255);
    }
  }

  for (int i = sel->top_left.line; i < sel->bot_right.line; ++i) {
    for (int j = sel->top_left.col; j < sel->bot_right.col; ++j) {
      ((ppm_point_t **)img_file->mat)[i][j].red =
          copy[i - sel->top_left.line][j - sel->top_left.col].red;
      ((ppm_point_t **)img_file->mat)[i][j].green =
          copy[i - sel->top_left.line][j - sel->top_left.col].green;
      ((ppm_point_t **)img_file->mat)[i][j].blue =
          copy[i - sel->top_left.line][j - sel->top_left.col].blue;
    }
  }

  for (int i = 0; i < sel_height; ++i) {
    free(copy[i]);
  }
  free(copy);

  for (int i = 0; i < 3; ++i) {
    free(mat[i]);
  }
  free(mat);

  return 1;
}