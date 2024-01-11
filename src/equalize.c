// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "math_utils.h"
#include "structs.h"

// aceasta functie executa operatia de egalizare
int equalize_image(image_file_t *img_file)
{
	// am creat un vector de frecventa initializat pe 0
	int *freq = calloc(256, sizeof(int));

	// pentru fiecare punct din imagine, incrementez valoarea din vectorul de
	// frecventa
	for (int i = 0; i < img_file->height; ++i) {
		for (int j = 0; j < img_file->width; ++j) {
			int grayscale_point = ((pgm_point_t **)img_file->mat)[i][j].grey;

			++freq[grayscale_point];
		}
	}

	// calculez sume partiale, deoarece sunt necesare in formula de egalizare
	int *sp = calloc(256, sizeof(int));
	sp[0] = freq[0];

	for (int i = 1; i < 256; ++i)
		sp[i] = sp[i - 1] + freq[i];

	int area = sp[255];

	for (int i = 0; i < img_file->height; ++i) {
		for (int j = 0; j < img_file->width; ++j) {
			int grayscale_point = ((pgm_point_t **)img_file->mat)[i][j].grey;

			int new_val = clamp(255 * sp[grayscale_point] / area, 0, 255);

			((pgm_point_t **)img_file->mat)[i][j].grey = new_val;
		}
	}

	free(freq);
	free(sp);

	return 1;
}
