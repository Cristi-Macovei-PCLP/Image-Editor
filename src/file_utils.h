// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

#ifndef GUARD_FILE_UTILS_H
#define GUARD_FILE_UTILS_H

#include <stdio.h>

#include "structs.h"

int parse_image_file(FILE * file, image_file_t *img_file);

void free_image_file(image_file_t *img_file);

void save_image_binary(image_file_t *img_file, char *filename);

void save_image_ascii(image_file_t *img_file, char *filename);

#endif
