#include <stdio.h>

#include "structs.h"

int parse_image_file(FILE *file, image_file_t *img_file);

void free_image_file(image_file_t *img_file);
