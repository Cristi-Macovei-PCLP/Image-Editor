#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "structs.h"

// check if character is blank (0x00), LF, CR, Whitespace, tab or newline
int __is_empty_character(char c) {
  return c == '\0' || c == 0x0a || c == 0x0d || c == ' ' || c == '\t' ||
         c == '\n';
}

void __read_one_point_data(FILE *file, int *ptr_data, int num_bytes) {
  char LF_CHAR = 0x0a;

  unsigned char byte1;
  fread(&byte1, 1 * sizeof(char), 1, file);

  // todo fix this
  // // if there's comments
  // while (byte1 == '#') {
  //   do {
  //     fread(&byte1, 1 * sizeof(char), 1, file);
  //   } while (byte1 != LF_CHAR);
  //   fread(&byte1, 1 * sizeof(char), 1, file);
  // }

  *ptr_data = (int)byte1;

  if (num_bytes == 2) {
    unsigned char byte2;
    fread(&byte2, 1 * sizeof(char), 1, file);

    // if there's comments
    while (byte2 == '#') {
      do {
        fread(&byte2, 1 * sizeof(char), 1, file);
      } while (byte2 != LF_CHAR);
      fread(&byte2, 1 * sizeof(char), 1, file);
    }

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

int __read__decimal_number(FILE *file) {
  char LF_CHAR = 0x0a;

  int ans = 0;
  while (!feof(file)) {
    char digit;
    fread(&digit, 1 * sizeof(char), 1, file);

    // if there's comments
    while (digit == '#') {
      do {
        fread(&digit, 1 * sizeof(char), 1, file);
      } while (digit != LF_CHAR);
      fread(&digit, 1 * sizeof(char), 1, file);
    }

    if (digit < '0' || digit > '9') {
      break;
    }

    ans = 10 * ans + (digit - '0');
  }

  return ans;
}

int parse_color_image(FILE *file, image_file_t *img_file) {
  img_file->type = IMAGE_COLOR;

  // after magic constants, there should be an empty char
  char empty;
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
  int width = __read__decimal_number(file);
  if (width == 0) {
    return 0;
  } else {
    img_file->width = width;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", width);
#endif

  int height = __read__decimal_number(file);
  if (height == 0) {
    return 0;
  } else {
    img_file->height = height;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image height: %d\n", height);
#endif

  int color_max_value = __read__decimal_number(file);
  if (color_max_value == 0) {
    return 0;
  } else {
    img_file->color_max_value = color_max_value;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image color max value: %d\n", color_max_value);
#endif

  img_file->mat = malloc(img_file->height * sizeof(ppm_point_t *));
  for (int line = 0; line < img_file->height; ++line) {
    img_file->mat[line] = malloc(img_file->width * sizeof(ppm_point_t));

    for (int col = 0; col < img_file->width; ++col) {
      read_ppm_point(file, &(((ppm_point_t **)(img_file->mat))[line][col]),
                     img_file->color_max_value > 255 ? 2 : 1);
    }
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Successfully read all %d x %d image points\n",
          img_file->width, img_file->height);
#endif

  return 1;
}

int parse_ascii_color_image(FILE *file, image_file_t *img_file) {
  img_file->type = IMAGE_COLOR;

  fscanf(file, "%d", &img_file->width);
  fscanf(file, "%d", &img_file->height);
  fscanf(file, "%d", &img_file->color_max_value);

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", img_file->width);
  fprintf(stderr, "[debug] Read image height: %d\n", img_file->height);
  fprintf(stderr, "[debug] Read image max_color: %d\n",
          img_file->color_max_value);
#endif

  img_file->mat = malloc(img_file->height * sizeof(ppm_point_t *));
  for (int line = 0; line < img_file->height; ++line) {
    img_file->mat[line] = malloc(img_file->width * sizeof(ppm_point_t));

    for (int col = 0; col < img_file->width; ++col) {
      fscanf(file, "%d", &(((ppm_point_t **)img_file->mat)[line][col].red));
      fscanf(file, "%d", &(((ppm_point_t **)img_file->mat)[line][col].green));
      fscanf(file, "%d", &(((ppm_point_t **)img_file->mat)[line][col].blue));
    }
  }

  return 1;
}

int parse_grayscale_image(FILE *file, image_file_t *img_file) {
  img_file->type = IMAGE_GRAYSCALE;

  // after magic constants, there should be an empty char
  char empty;
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
  int width = __read__decimal_number(file);
  if (width == 0) {
    return 0;
  } else {
    img_file->width = width;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", width);
#endif

  int height = __read__decimal_number(file);
  if (height == 0) {
    return 0;
  } else {
    img_file->height = height;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image height: %d\n", height);
#endif

  int color_max_value = __read__decimal_number(file);
  if (color_max_value == 0) {
    return 0;
  } else {
    img_file->color_max_value = color_max_value;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image color max value: %d\n", color_max_value);
#endif

  img_file->mat = malloc(img_file->height * sizeof(pgm_point_t *));
  for (int line = 0; line < img_file->height; ++line) {
    img_file->mat[line] = malloc(img_file->width * sizeof(pgm_point_t));

    for (int col = 0; col < img_file->width; ++col) {
      read_pgm_point(file, &(((pgm_point_t **)(img_file->mat))[line][col]),
                     img_file->color_max_value > 255 ? 2 : 1);
    }
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Successfully read all %d x %d image points\n",
          img_file->width, img_file->height);
#endif

  return 1;
}

int parse_ascii_grayscale_image(FILE *file, image_file_t *img_file) {
  img_file->type = IMAGE_GRAYSCALE;

  fscanf(file, "%d", &img_file->width);
  fscanf(file, "%d", &img_file->height);
  fscanf(file, "%d", &img_file->color_max_value);

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", img_file->width);
  fprintf(stderr, "[debug] Read image height: %d\n", img_file->height);
  fprintf(stderr, "[debug] Read image max_color: %d\n",
          img_file->color_max_value);
#endif

  img_file->mat = malloc(img_file->height * sizeof(pgm_point_t *));
  for (int line = 0; line < img_file->height; ++line) {
    img_file->mat[line] = malloc(img_file->width * sizeof(pgm_point_t));

    for (int col = 0; col < img_file->width; ++col) {
      fscanf(file, "%d", &(((pgm_point_t **)img_file->mat)[line][col].grey));
    }
  }

  return 1;
}

int parse_black_white_image(FILE *file, image_file_t *img_file) {
  img_file->type = IMAGE_BLACK_WHITE;

  // after magic constants, there should be an empty char
  char empty;
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
  int width = __read__decimal_number(file);
  if (width == 0) {
    return 0;
  } else {
    img_file->width = width;
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image width: %d\n", width);
#endif

  int height = __read__decimal_number(file);
  if (height == 0) {
    return 0;
  } else {
    img_file->height = height;
  }

  img_file->color_max_value = 255;

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Read image height: %d\n", height);
#endif

  img_file->mat = malloc(img_file->height * sizeof(ppm_point_t *));
  for (int i = 0; i < img_file->height; ++i) {
    img_file->mat[i] = malloc(img_file->width * sizeof(ppm_point_t));
  }

  int num_points = width * height;
  int c_line = 0, c_col = 0;
  for (int i = 0; i < num_points; i += 8) {
    unsigned char byte;
    fread(&byte, 1, 1, file);

    // printf("Byte before = %x  ", byte);

    while (byte == '#') {
      do {
        fread(&byte, 1 * sizeof(char), 1, file);
      } while (byte != 0x0a);
      fread(&byte, 1 * sizeof(char), 1, file);
    }

    // printf("Byte after = %x\n", byte);

    for (int bit = 7; bit >= 0; --bit) {
      int value = byte & (1 << bit);

      int point_val = 0;
      if (value) {
        point_val = 1;
      }

      ((pgm_point_t **)img_file->mat)[c_line][c_col].grey = point_val;

      ++c_col;
      if (c_col >= img_file->width) {
        c_col -= img_file->width;
        ++c_line;
      }
    }
  }

#ifdef MODE_DEBUG
  fprintf(stderr, "[debug] Successfully read all %d x %d image points\n",
          img_file->width, img_file->height);
#endif

  return 1;
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

    return parse_color_image(file, img_file);
  } else if (magic[0] == 'P' && magic[1] == '5') {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants for PGM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif

    return parse_grayscale_image(file, img_file);
  } else if (magic[0] == 'P' && magic[1] == '4') {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants for PBM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif

    return parse_black_white_image(file, img_file);
  } else if (magic[0] == 'P' && magic[1] == '3') {
#ifdef MODE_DEBUG
    fprintf(stderr,
            "[debug] Magic constants for ASCII PPM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif
    return parse_ascii_grayscale_image(file, img_file);
  } else if (magic[0] == 'P' && magic[1] == '2') {
#ifdef MODE_DEBUG
    fprintf(stderr,
            "[debug] Magic constants for ASCII PGM image found (0x%x 0x%x)\n",
            magic[0], magic[1]);
#endif
    return parse_ascii_grayscale_image(file, img_file);
  } else {
#ifdef MODE_DEBUG
    fprintf(stderr, "[debug] Magic constants invalid, found 0x%x 0x%x\n",
            magic[0], magic[1]);
#endif

    return 0;
  }
}

char *itoa(int a) {
  int nd = 0;
  int _a = a;

  do {
    ++nd;
    _a /= 10;
  } while (_a);

  char *string = malloc((1 + nd) * sizeof(char));

  int index = nd;
  do {
    int digit = a % 10;
    a /= 10;

    --index;
    string[index] = (char)(digit + '0');
  } while (a);

  string[nd] = '\0';

  return string;
}

void __save_color_image_binary(image_file_t *img_file, char *filename) {
  FILE *file = fopen(filename, "wb");

  char LF_CHAR = 0x0a;
  char SPC_CHAR = ' ';

  // write magic constants
  fwrite("P6", 2, 1, file);
  fwrite(&LF_CHAR, 1, 1, file);

  char *string_width = itoa(img_file->width);
  fwrite(string_width, strlen(string_width), 1, file);
  free(string_width);

  fwrite(&SPC_CHAR, 1, 1, file);

  char *string_height = itoa(img_file->height);
  fwrite(string_height, strlen(string_height), 1, file);
  // printf("Free string_height = '%s' (%p)\n", string_height, string_height);
  free(string_height);

  fwrite(&LF_CHAR, 1, 1, file);

  char *string_max_val = itoa(img_file->color_max_value);
  fwrite(string_max_val, strlen(string_max_val), 1, file);
  free(string_max_val);

  fwrite(&LF_CHAR, 1, 1, file);

  for (int i = 0; i < img_file->height; ++i) {
    for (int j = 0; j < img_file->width; ++j) {
      // red
      if (img_file->color_max_value <= 255) {
        unsigned char value = ((ppm_point_t **)img_file->mat)[i][j].red;
        fwrite(&value, 1, 1, file);
      } else {
        unsigned short value = ((ppm_point_t **)img_file->mat)[i][j].red;
        unsigned char byte1 = value / 0xff;
        unsigned char byte2 = value % 0xff;

        fwrite(&byte1, 1, 1, file);
        fwrite(&byte2, 1, 1, file);
      }

      // green
      if (img_file->color_max_value <= 255) {
        unsigned char value = ((ppm_point_t **)img_file->mat)[i][j].green;
        fwrite(&value, 1, 1, file);
      } else {
        unsigned short value = ((ppm_point_t **)img_file->mat)[i][j].green;
        unsigned char byte1 = value / 0xff;
        unsigned char byte2 = value % 0xff;

        fwrite(&byte1, 1, 1, file);
        fwrite(&byte2, 1, 1, file);
      }

      // red
      if (img_file->color_max_value <= 255) {
        unsigned char value = ((ppm_point_t **)img_file->mat)[i][j].blue;
        fwrite(&value, 1, 1, file);
      } else {
        unsigned short value = ((ppm_point_t **)img_file->mat)[i][j].blue;
        unsigned char byte1 = value / 0xff;
        unsigned char byte2 = value % 0xff;

        fwrite(&byte1, 1, 1, file);
        fwrite(&byte2, 1, 1, file);
      }
    }
  }

  fclose(file);
}

void __save_grayscale_image_binary(image_file_t *img_file, char *filename) {
  FILE *file = fopen(filename, "wb");

  char LF_CHAR = 0x0a;
  char SPC_CHAR = ' ';

  // write magic constants
  fwrite("P5", 2, 1, file);
  fwrite(&LF_CHAR, 1, 1, file);

  char *string_width = itoa(img_file->width);
  fwrite(string_width, strlen(string_width), 1, file);
  free(string_width);

  fwrite(&SPC_CHAR, 1, 1, file);

  char *string_height = itoa(img_file->height);
  fwrite(string_height, strlen(string_height), 1, file);
  free(string_height);

  fwrite(&LF_CHAR, 1, 1, file);

  char *string_max_val = itoa(img_file->color_max_value);
  fwrite(string_max_val, strlen(string_max_val), 1, file);
  free(string_max_val);

  fwrite(&LF_CHAR, 1, 1, file);

  for (int i = 0; i < img_file->height; ++i) {
    for (int j = 0; j < img_file->width; ++j) {
      if (img_file->color_max_value <= 255) {
        unsigned char value = ((pgm_point_t **)img_file->mat)[i][j].grey;
        fwrite(&value, 1, 1, file);
      } else {
        unsigned short value = ((pgm_point_t **)img_file->mat)[i][j].grey;
        unsigned char byte1 = value / 0xff;
        unsigned char byte2 = value % 0xff;

        fwrite(&byte1, 1, 1, file);
        fwrite(&byte2, 1, 1, file);
      }
    }
  }

  fclose(file);
}

void __save_black_white_image_binary(image_file_t *img_file, char *filename) {
  FILE *file = fopen(filename, "wb");

  char LF_CHAR = 0x0a;
  char SPC_CHAR = ' ';

  fwrite("P4", 2, 1, file);
  fwrite(&LF_CHAR, 1, 1, file);

  char *string_width = itoa(img_file->width);
  fwrite(string_width, strlen(string_width), 1, file);
  free(string_width);

  fwrite(&SPC_CHAR, 1, 1, file);

  char *string_height = itoa(img_file->height);
  fwrite(string_height, strlen(string_height), 1, file);
  free(string_height);

  fwrite(&LF_CHAR, 1, 1, file);

  int num_points = img_file->width * img_file->height;
  int c_line = 0, c_col = 0;
  for (int i = 0; i < num_points; i += 8) {
    unsigned char byte = 0;

    for (int bit = 7; bit >= 0; --bit) {
      int point_val = ((pgm_point_t **)img_file->mat)[c_line][c_col].grey;

      if (point_val) {
        byte |= (1 << bit);
      }

      ++c_col;
      if (c_col >= img_file->width) {
        c_col -= img_file->width;
        ++c_line;
      }
    }

    fwrite(&byte, 1, 1, file);
  }

  fclose(file);
}

void __save_color_image_ascii(image_file_t *img_file, char *filename) {
  FILE *file = fopen(filename, "w");

  fprintf(file, "P3\n");
  fprintf(file, "%d %d\n", img_file->width, img_file->height);
  fprintf(file, "%d\n", img_file->color_max_value);

  for (int i = 0; i < img_file->height; ++i) {
    for (int j = 0; j < img_file->width; ++j) {
      fprintf(file, "%d ", ((ppm_point_t **)img_file->mat)[i][j].red);
      fprintf(file, "%d ", ((ppm_point_t **)img_file->mat)[i][j].green);
      fprintf(file, "%d ", ((ppm_point_t **)img_file->mat)[i][j].blue);
    }
    fprintf(file, "\n");
  }

  fclose(file);
}

void __save_grayscale_image_ascii(image_file_t *img_file, char *filename) {
  FILE *file = fopen(filename, "w");

  fprintf(file, "P2\n");
  fprintf(file, "%d %d\n", img_file->width, img_file->height);
  fprintf(file, "%d\n", img_file->color_max_value);

  for (int i = 0; i < img_file->height; ++i) {
    for (int j = 0; j < img_file->width; ++j) {
      fprintf(file, "%d ", ((pgm_point_t **)img_file->mat)[i][j].grey);
    }
    fprintf(file, "\n");
  }

  fclose(file);
}

void save_image_binary(image_file_t *img_file, char *filename) {
  if (img_file->type == IMAGE_GRAYSCALE) {
    __save_grayscale_image_binary(img_file, filename);
  } else if (img_file->type == IMAGE_COLOR) {
    __save_color_image_binary(img_file, filename);
  } else if (img_file->type == IMAGE_BLACK_WHITE) {
    __save_black_white_image_binary(img_file, filename);
  }
}

void save_image_ascii(image_file_t *img_file, char *filename) {
  if (img_file->type == IMAGE_GRAYSCALE) {
    __save_grayscale_image_ascii(img_file, filename);
  }
}

void free_image_file(image_file_t *img_file) {
  if (img_file->filename != NULL) {
    free(img_file->filename);
    img_file->filename = NULL;
  }

  if (img_file->mat != NULL) {
    for (int i = 0; i < img_file->height; ++i) {
      free(img_file->mat[i]);
    }
    free(img_file->mat);

    img_file->mat = NULL;
  }
}