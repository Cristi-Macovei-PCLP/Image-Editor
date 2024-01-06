// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

#ifdef MODE_DEBUG
void debug_check_point(image_file_t *img_file, int i, int j)
{
	if (img_file->type == IMAGE_COLOR) {
		if (((ppm_point_t **)img_file->mat)[i][j].red >= 256 ||
			((ppm_point_t **)img_file->mat)[i][j].red < 0)
			fprintf(stderr,
					"[debug] Crop - invalid red for point %d %d\n",
					i, j);
		if (((ppm_point_t **)img_file->mat)[i][j].green >= 256 ||
			((ppm_point_t **)img_file->mat)[i][j].green < 0)
			fprintf(stderr,
					"[debug] Crop - invalid green for point %d %d\n",
					i, j);
		if (((ppm_point_t **)img_file->mat)[i][j].blue >= 256 ||
			((ppm_point_t **)img_file->mat)[i][j].blue < 0)
			fprintf(stderr,
					"[debug] Crop - invalid blue for point %d %d\n",
					i, j);
	} else if (img_file->type == IMAGE_GRAYSCALE) {
		if (((pgm_point_t **)img_file->mat)[i][j].grey >= 256 ||
			((pgm_point_t **)img_file->mat)[i][j].grey < 0)
			fprintf(stderr,
					"[debug] Crop - invalid grey for point %d %d\n",
					i, j);
	}
}
#endif

int crop_image(image_file_t *img_file, selection_t *sel)
{
	if (sel->top_left.line < 0 || sel->top_left.line >= img_file->height)
		return 0;
	if (sel->top_left.col < 0 || sel->top_left.col >= img_file->width)
		return 0;
	if (sel->top_left.line < 0 || sel->top_left.line >= img_file->height)
		return 0;
	if (sel->top_left.col < 0 || sel->top_left.col >= img_file->width)
		return 0;

	int new_width = sel->bot_right.col - sel->top_left.col;
	int new_height = sel->bot_right.line - sel->top_left.line;

	int old_height = img_file->height;

	img_file->width = new_width;
	img_file->height = new_height;

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] New image width: %d, new height: %d\n",
			new_width, new_height);
#endif

	for (int i = 0; i < new_height; ++i) {
		for (int j = 0; j < new_width; ++j) {
			int line = sel->top_left.line + i;
			int col = sel->top_left.col + j;

			if (img_file->type == IMAGE_GRAYSCALE) {
				((pgm_point_t **)img_file->mat)[i][j].grey =
					((pgm_point_t **)img_file->mat)[line][col].grey;
			} else {
				((ppm_point_t **)img_file->mat)[i][j].red =
					((ppm_point_t **)img_file->mat)[line][col].red;

				((ppm_point_t **)img_file->mat)[i][j].green =
					((ppm_point_t **)img_file->mat)[line][col].green;

				((ppm_point_t **)img_file->mat)[i][j].blue =
					((ppm_point_t **)img_file->mat)[line][col].blue;
			}

#ifdef MODE_DEBUG
			debug_check_point(img_file, i, j);
#endif
		}
	}

	// free memory
	for (int i = 0; i < new_height; ++i) {
		void *new_line = realloc(img_file->mat[i], new_width *
						 (img_file->type == IMAGE_COLOR ?
						 sizeof(ppm_point_t) : sizeof(pgm_point_t)));
		if (!new_line) {
			fprintf(stderr, "A plesnit realloc-u :(\n");
			exit(-1);
		}

		img_file->mat[i] = new_line;
	}
	for (int i = new_height; i < old_height; ++i)
		free(img_file->mat[i]);

	void **new_mat = realloc(img_file->mat, new_height * sizeof(void *));
	if (!new_mat) {
		fprintf(stderr, "A plesnit realloc-u :(\n");
		exit(-1);
	}
	img_file->mat = new_mat;

	return 1;
}
