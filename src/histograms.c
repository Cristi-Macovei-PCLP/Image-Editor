#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

// todo vezi ca nu merge panarama
// has to be a power of 2 between 2 and 256
int check_histogram_bin_value(int val) {
  return 2 <= val && val <= 256 && ((val - 1) ^ (~val)) == 0;
}

void print_histogram(image_file_t *img_file, int stars, int bins) {
  // if (!check_histogram_bin_value(bins)) {
  //   fprintf(stderr, "Invalid bin value '%d'\n", bins);
  //   return;
  // }

  int *freq = malloc(bins * sizeof(int));
  for (int i = 0; i < bins; ++i) {
    freq[i] = 0;
  }

  int num_values_per_bin = (1 + img_file->color_max_value) / bins;

  for (int i = 0; i < img_file->height; ++i) {
    for (int j = 0; j < img_file->width; ++j) {
      int grayscale_point = ((pgm_point_t **)(img_file->mat))[i][j].grey;

      int bin = grayscale_point / num_values_per_bin;
      ++freq[bin];

#ifdef MODE_DEBUG
      fprintf(stderr, "Grayscale point %d falls into bin %d\n", grayscale_point,
              bin);
#endif
    }
  }

  int max_freq = freq[0];
  for (int i = 1; i < bins; ++i) {
    if (freq[i] > max_freq) {
      max_freq = freq[i];
    }
  }

  // max_freq ... stars
  // freq     ... x
  // x = freq * stars / max_freq
  for (int bin = 0; bin < bins; ++bin) {
    int num_stars = freq[bin] * stars / max_freq;

    printf("%d\t|\t", num_stars);
    for (int i = 0; i < num_stars; ++i) {
      printf("*");
    }
    printf("\n");
  }

  free(freq);
}