#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apply_param.h"
#include "cmd_utils.h"
#include "crop.h"
#include "defines.h"
#include "equalize.h"
#include "file_utils.h"
#include "histograms.h"
#include "rotate.h"
#include "structs.h"

int main(void)
{
	char cmd_buffer[1 + CMD_BUFFER_SIZE];

	int has_file = 0;
	image_file_t current_file = {NULL, 0, 0, 0, -1, NULL};

	int has_sel = 0;
	selection_t current_sel = {{0, 0}, {0, 0}, 0};

	while (!feof(stdin)) {
		// printf("> ");
		fgets(cmd_buffer, CMD_BUFFER_SIZE * sizeof(char), stdin);

		// remove trailing '\n'
		if (cmd_buffer[strlen(cmd_buffer) - 1] == '\n')
			cmd_buffer[strlen(cmd_buffer) - 1] = '\0';

#ifdef MODE_DEBUG
		fprintf(stderr, "[debug] Command: '%s'\n", cmd_buffer);
#endif

		// load command
		if (check_command(cmd_buffer, "LOAD")) {
			// free loaded file if exists
			if (has_file) {
#ifdef MODE_DEBUG
				fprintf(stderr, "[debug] Freeing existing file\n");
#endif
				free_image_file(&current_file);
			}

			char *filename = NULL;
			get_load_cmd_arg(cmd_buffer, &filename);

			if (!filename) {
				printf("No filename for 'LOAD' found\n");
				has_file = 0;
				if (filename)
					free(filename);
				continue;
			}

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Loading file: '%s'\n", filename);
#endif

			FILE * file = fopen(filename, "rb");

			if (!file) {
				printf("Failed to load %s\n", filename);
				has_file = 0;
				free(filename);
				continue;
			}

			current_file.filename = filename;

			has_file = 1;
			if (!parse_image_file(file, &current_file)) {
				printf("Failed to load %s\n", filename);
				has_file = 0;

				free_image_file(&current_file);
				fclose(file);

				continue;
			}

			printf("Loaded %s\n", filename);

			// after image is loaded, automatically select all
			has_sel = 1;
			current_sel.is_all = 1;
			current_sel.top_left.line = 0;
			current_sel.top_left.col = 0;
			current_sel.bot_right.line = current_file.height;
			current_sel.bot_right.col = current_file.width;

			fclose(file);
		}

#ifdef MODE_DEBUG
		else if (check_command(cmd_buffer, "SHOW")) {
			fprintf(stderr, "[debug] Loaded filename: %s\n",
					current_file.filename);
			fprintf(stderr, "[debug] Image size: h = %d, w = %d\n",
					current_file.height, current_file.width);
			fprintf(stderr, "[debug] Current selection: %d, %d --> %d, %d\n",
					current_sel.top_left.line, current_sel.top_left.col,
					current_sel.bot_right.line, current_sel.bot_right.col);
		}
#endif

		else if (check_command(cmd_buffer, "SELECT")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			selection_t new_sel;
			int is_selection_valid =
					get_select_cmd_args(cmd_buffer, &new_sel, &current_file);

			if (is_selection_valid == 1) {
				has_sel = 1;
				current_sel = new_sel;
			} else if (is_selection_valid == 0) {
				printf("Invalid set of coordinates\n");
				continue;
			} else {
				printf("Invalid command\n");
				continue;
			}

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Selected %d,%d --> %d,%d\n",
					current_sel.top_left.line, current_sel.top_left.col,
					current_sel.bot_right.line, current_sel.bot_right.col);
#endif

			if (current_sel.is_all) {
				printf("Selected ALL\n");
			} else {
				printf("Selected %d %d %d %d\n", current_sel.top_left.col,
					   current_sel.top_left.line, current_sel.bot_right.col,
					   current_sel.bot_right.line);
			}
		}

		else if (check_command(cmd_buffer, "CROP")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			if (!has_sel) {
				fprintf(stderr, "No selection set\n");
				continue;
			}

			crop_image(&current_file, &current_sel);

			current_sel.is_all = 1;
			current_sel.top_left.line = 0;
			current_sel.top_left.col = 0;
			current_sel.bot_right.line = current_file.height;
			current_sel.bot_right.col = current_file.width;

			printf("Image cropped\n");
		}

		else if (check_command(cmd_buffer, "ROTATE")) {
			int angle = -1;
			get_rotate_cmd_args(cmd_buffer, &angle);

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Rotate angle = %d\n", angle);
#endif

			if (abs(angle) != 0 && abs(angle) != 90 && abs(angle) != 180 &&
				abs(angle) != 270 && abs(angle) != 360) {
				printf("Unsupported rotation angle\n");
				continue;
			}

			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			if (!has_sel) {
				fprintf(stderr, "No selection set");
				continue;
			}

			if (current_sel.is_all ||
				(current_sel.top_left.line == 0 &&
				 current_sel.top_left.col == 0 &&
				 current_sel.bot_right.line == current_file.height &&
				 current_sel.bot_right.col == current_file.width)) {
				rotate_all(&current_file, &current_sel, angle);
			} else {
				rotate_square(&current_file, &current_sel, angle);
			}

			printf("Rotated %d\n", angle);
		}

		else if (check_command(cmd_buffer, "HISTOGRAM")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			int x, y;
			int is_valid = get_histogram_cmd_args(cmd_buffer, &x, &y);

			if (is_valid == 0) {
				printf("Invalid command\n");
				continue;
			}

			if (current_file.type != IMAGE_GRAYSCALE) {
				printf("Black and white image needed\n");
				continue;
			}

			print_histogram(&current_file, x, y);
		}

		else if (check_command(cmd_buffer, "APPLY")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			int apply_param_type = APPLY_PARAM_NON_EXISTENT;
			get_apply_cmd_args(cmd_buffer, &apply_param_type);

			if (apply_param_type == APPLY_PARAM_NON_EXISTENT) {
				printf("Invalid command\n");
				continue;
			}

			if (apply_param_type == APPLY_PARAM_BAD) {
				printf("APPLY parameter invalid\n");
				continue;
			}

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Param type: %d\n", apply_param_type);
#endif

			if (current_file.type == IMAGE_GRAYSCALE) {
				printf("Easy, Charlie Chaplin\n");
				continue;
			}

			apply_param(&current_file, &current_sel, apply_param_type);

			if (apply_param_type == APPLY_PARAM_EDGE)
				printf("APPLY EDGE done\n");
			else if (apply_param_type == APPLY_PARAM_SHARPEN)
				printf("APPLY SHARPEN done\n");
			else if (apply_param_type == APPLY_PARAM_BLUR)
				printf("APPLY BLUR done\n");
			else if (apply_param_type == APPLY_PARAM_GAUSSIAN_BLUR)
				printf("APPLY GAUSSIAN_BLUR done\n");
		}

		else if (check_command(cmd_buffer, "EQUALIZE")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			if (current_file.type != IMAGE_GRAYSCALE) {
				printf("Black and white image needed\n");
				continue;
			}

			equalize_image(&current_file);

			printf("Equalize done\n");
		}

		else if (check_command(cmd_buffer, "SAVE")) {
			if (!has_file) {
				printf("No image loaded\n");
				continue;
			}

			char *filename = NULL;
			int is_ascii = 0;
			get_save_cmd_args(cmd_buffer, &filename, &is_ascii);

			if (!filename) {
				printf("No filename given\n");
				continue;
			}

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Saving to %s\n", filename);
#endif

#ifdef MODE_DEBUG
			fprintf(stderr, "[debug] Is ascii: %d\n", is_ascii);
#endif

			if (is_ascii)
				save_image_ascii(&current_file, filename);
			else
				save_image_binary(&current_file, filename);

			printf("Saved %s\n", filename);

			free(filename);
		}

		else if (check_command(cmd_buffer, "EXIT")) {
#ifdef MODE_DEBUG
			printf("[debug] Exiting program with %sloaded file\n",
				   has_file ? "1 " : "no ");
#endif
			if (has_file)
				free_image_file(&current_file);
			else
				printf("No image loaded\n");

			return 0;
		} else {
			printf("Invalid command\n");
			continue;
		}
	}

	return 0;
}
