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

int apply_param(image_file_t *img_file, int param_type) {
  double **mat = __get_apply_mat(param_type);

  printf("MERGE PANA AICI 1\n");

  ppm_point_t **copy = malloc(img_file->height * sizeof(ppm_point_t *));
  for (int i = 0; i < img_file->height; ++i) {
    copy[i] = malloc(img_file->width * sizeof(ppm_point_t));
  }

  for (int i = 1; i < img_file->height - 1; ++i) {
    for (int j = 1; j < img_file->width - 1; ++j) {
      double new_red = 0;
      double new_green = 0;
      double new_blue = 0;
      for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
          new_red += ((ppm_point_t **)img_file->mat)[i + di][j + dj].red *
                     mat[1 + di][1 + dj];
          new_green += ((ppm_point_t **)img_file->mat)[i + di][j + dj].green *
                       mat[1 + di][1 + dj];
          new_blue += ((ppm_point_t **)img_file->mat)[i + di][j + dj].blue *
                      mat[1 + di][1 + dj];
        }
      }

      copy[i][j].red = clamp((int)new_red, 0, 255);
      copy[i][j].green = clamp((int)new_green, 0, 255);
      copy[i][j].blue = clamp((int)new_blue, 0, 255);
    }
  }

  for (int i = 0; i < img_file->height; ++i) {
    copy[i][0].red = copy[i][img_file->width - 1].red = 0;
    copy[i][0].green = copy[i][img_file->width - 1].green = 0;
    copy[i][0].blue = copy[i][img_file->width - 1].blue = 0;
  }

  for (int i = 0; i < img_file->width; ++i) {
    copy[0][i].red = copy[img_file->height - 1][i].red = 0;
    copy[0][i].green = copy[img_file->height - 1][i].green = 0;
    copy[0][i].blue = copy[img_file->height - 1][i].blue = 0;
  }

  printf("MERGE PANA AICI 2\n");

  for (int i = 0; i < img_file->height; ++i) {
    free(img_file->mat[i]);
  }
  free(img_file->mat);

  printf("MERGE PANA AICI 3\n");

  img_file->mat = (void **)copy;

  for (int i = 0; i < 3; ++i) {
    free(mat[i]);
  }
  free(mat);

  return 1;
}