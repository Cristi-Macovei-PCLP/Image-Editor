#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

// check if character is blank (0x00), LF, CR, Whitespace, tab or newline
int __is_empty_character(char c) {
  return c == '\0' || c == 0x0a || c == 0x0d || c == ' ' || c == '\t' ||
         c == '\n';
}

void __read_one_point_data(FILE *file, int *ptr_data, int num_bytes) {
  char byte1;
  fread(&byte1, 1 * sizeof(char), 1, file);

  *ptr_data = (int)byte1;

  if (num_bytes == 2) {
    char byte2;
    fread(&byte2, 1 * sizeof(char), 1, file);

    *ptr_data = 0xff * (*ptr_data) + (int)byte2;
  }
}

void read_pgm_point(FILE *file, pgm_point_t *ptr_point, int num_bytes) {
  __read_one_point_data(file, &(ptr_point->grey), num_bytes);
}

void read_ppm_point(FILE *file, ppm_point_t *ptr_point, int num_bytes) {
  __read_one_point_data(file, &(ptr_point->red), num_bytes);
  __read_one_point_data(file, &(ptr_point->green), num_bytes);
  __read_one_point_data(file, &(ptr_point->blue), num_bytes);
}

int parse_image_file(FILE *file, image_file_t *img_file) {
  char magic[2];
  char empty;

  // read magic numbers
  fread(magic, 2 * sizeof(char), 1, file);

  if (magic[0] == 'P' && magic[1] == '6') {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants for PPM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif

    img_file->type = IMAGE_TYPE_PPM;
  } else if (magic[0] == 'P' && magic[1] == '5') {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants for PGM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif
    img_file->type = IMAGE_TYPE_PGM;
  } else {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants invalid, found 0x%x 0x%x\n",
            magic[0], magic[1]);
#endif

    return 0;
  }

  fread(&empty, 1 * sizeof(char), 1, file);
  if (!__is_empty_character(empty)) {
#ifdef MODE_DEBUG
    fprintf(stderr,
            "[debug] Byte #3 invalid, found %x, expected empty character\n",
            empty);
#endif
    return 0;
  }

  // read width
  int width = 0;
  while (!feof(file)) {
    char digit;
    fread(&digit, 1 * sizeof(char), 1, file);

    if (digit < '0' || digit > '9') {
      break;
    }

    width = 10 * width + (digit - '0');
  }

  if (width == 0) {
    return 0;
  } else {
    img_file->width = width;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", width);
#endif

  // read height
  int height = 0;
  while (!feof(file)) {
    char digit;
    fread(&digit, 1 * sizeof(char), 1, file);

    if (digit < '0' || digit > '9') {
      break;
    }

    height = 10 * height + (digit - '0');
  }

  if (height == 0) {
    return 0;
  } else {
    img_file->height = height;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image height: %d\n", height);
#endif

  // read color max value
  int color_max_value = 0;
  while (!feof(file)) {
    char digit;
    fread(&digit, 1 * sizeof(char), 1, file);

    if (digit < '0' || digit > '9') {
      break;
    }

    color_max_value = 10 * color_max_value + (digit - '0');
  }

  if (color_max_value == 0) {
    return 0;
  } else {
    img_file->color_max_value = color_max_value;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image color max value: %d\n", color_max_value);
#endif

  // alloc memory for matrix
  if (img_file->type == IMAGE_TYPE_PPM) {
    img_file->mat = malloc(img_file->height * sizeof(ppm_point_t *));
  } else { // img_file->type == IMAGE_TYPE_PGM
    img_file->mat = malloc(img_file->height * sizeof(pgm_point_t *));
  }

  for (int line = 0; line < img_file->height; ++line) {
    if (img_file->type == IMAGE_TYPE_PPM) {
      img_file->mat[line] = malloc(img_file->width * sizeof(ppm_point_t));
    } else { // img_file->type == IMAGE_TYPE_PGM
      img_file->mat[line] = malloc(img_file->width * sizeof(pgm_point_t));
    }

    for (int col = 0; col < img_file->width; ++col) {
      if (img_file->type == IMAGE_TYPE_PPM) {
        read_ppm_point(file, &(((ppm_point_t **)(img_file->mat))[line][col]),
                       img_file->color_max_value > 255 ? 2 : 1);
      } else { // img_file->type == IMAGE_TYPE_PGM
        read_pgm_point(file, &(((pgm_point_t **)(img_file->mat))[line][col]),
                       img_file->color_max_value > 255 ? 2 : 1);
      }
    }
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Successfully read all %d x %d image points\n",
          img_file->width, img_file->height);
#endif

  return 1;
}