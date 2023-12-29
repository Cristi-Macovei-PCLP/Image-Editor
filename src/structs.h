// this thing is needed for the preprocessor to not write this header multiple
// times and throw an error because structures are being re-defined
#pragma once

typedef struct ppm_point {
	int red;
	int green;
	int blue;
} ppm_point_t;

typedef struct pgm_point {
	int grey;
} pgm_point_t;

typedef struct image_file {
	char *filename;
	int width, height;
	int color_max_value;
	int type;
	void **mat;
} image_file_t;

typedef struct position {
	int line, col;
} position_t;

typedef struct selection {
	position_t top_left;
	position_t bot_right;
	int is_all;
} selection_t;
