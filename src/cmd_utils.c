#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "structs.h"

int check_command(char *cmd_buffer, char *cmd_name)
{
	if (strlen(cmd_buffer) < strlen(cmd_name))
		return 0;

	// i know strcmp is a thing, i just don't like it
	for (int i = 0; i < (int)strlen(cmd_name); ++i)
		if (cmd_buffer[i] != cmd_name[i])
			return 0;

	if (isalnum(cmd_buffer[strlen(cmd_name)]))
		return 0;

	return 1;
}

void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;
	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		if (arg_index == 1) {
			char *filename = malloc(1 + strlen(p) * sizeof(char));
			strcpy(filename, p);

			*ptr_filename = filename;
			return;
		}

		++arg_index;
		p = strtok(NULL, " ");
	}
}

int is_numerical_string(char *string)
{
	int n = strlen(string);

	if (string[0] != '-' && !isdigit(string[0]))
		return 0;

	for (int i = 1; i < n; ++i)
		if (!isdigit(string[i]))
			return 0;

	return 1;
}

// returns
// 0 if coordinates are invalid
// 1 if everything is fine
// 2 if there's not 4 numerical coordinates
int get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel,
						image_file_t *img_file)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;
	ptr_sel->is_all = 0;
	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

#ifdef MODE_DEBUG
		fprintf(stderr, "[debug] select argument %d = '%s'\n", arg_index, p);
#endif

		if (arg_index == 1) {
			if (strcmp(p, "ALL") == 0) {
				ptr_sel->is_all = 1;

				ptr_sel->top_left.line = 0;
				ptr_sel->top_left.col = 0;
				ptr_sel->bot_right.line = img_file->height;
				ptr_sel->bot_right.col = img_file->width;
				return 1;
			}

			if (!is_numerical_string(p))
				return 2;

			ptr_sel->top_left.col = atoi(p);

			if (ptr_sel->top_left.col < 0 ||
				ptr_sel->top_left.col >= img_file->width) {
				return 0;
			}
		} else if (arg_index == 2) {
			if (!is_numerical_string(p))
				return 2;

			ptr_sel->top_left.line = atoi(p);

			if (ptr_sel->top_left.line < 0 ||
				ptr_sel->top_left.line >= img_file->height) {
				return 0;
			}
		} else if (arg_index == 3) {
			if (!is_numerical_string(p))
				return 2;

			ptr_sel->bot_right.col = atoi(p);

			if (ptr_sel->bot_right.col < 0 ||
				ptr_sel->bot_right.col > img_file->width)
				return 0;
		} else if (arg_index == 4) {
			if (!is_numerical_string(p))
				return 2;

			ptr_sel->bot_right.line = atoi(p);

			if (ptr_sel->bot_right.line < 0 ||
				ptr_sel->bot_right.line > img_file->height)
				return 0;
	}

	++arg_index;
	p = strtok(NULL, " ");
}

	if (arg_index <= 4)
		return 2;

	ptr_sel->is_all = 0;
	if (ptr_sel->top_left.line == ptr_sel->bot_right.line ||
		ptr_sel->top_left.col == ptr_sel->bot_right.col) {
		return 0;
	}

	if (ptr_sel->top_left.line > ptr_sel->bot_right.line) {
		int tmp = ptr_sel->top_left.line;
		ptr_sel->top_left.line = ptr_sel->bot_right.line;
		ptr_sel->bot_right.line = tmp;
	}

	if (ptr_sel->top_left.col > ptr_sel->bot_right.col) {
		int tmp = ptr_sel->top_left.col;
		ptr_sel->top_left.col = ptr_sel->bot_right.col;
		ptr_sel->bot_right.col = tmp;
	}

	return 1;
}

int get_histogram_cmd_args(char *cmd_buffer, int *ptr_x, int *ptr_y)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;
	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		if (arg_index == 1) {
			if (!is_numerical_string(p))
				return 0;

			*ptr_x = atoi(p);
		} else if (arg_index == 2) {
			if (!is_numerical_string(p))
				return 0;

			*ptr_y = atoi(p);
		}

		++arg_index;
		p = strtok(NULL, " ");
	}

	return arg_index == 3;
}

void get_save_cmd_args(char *cmd_buffer, char **ptr_filename,
					   int *ptr_is_ascii)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;

	*ptr_is_ascii = 0;

	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		if (arg_index == 1) {
			char *filename = malloc((1 + strlen(p)) * sizeof(char));
			strcpy(filename, p);
			*ptr_filename = filename;
		} else if (arg_index == 2) {
			if (strcmp(p, "ascii") == 0)
				*ptr_is_ascii = 1;
		}

		++arg_index;
		p = strtok(NULL, " ");
	}
}

void get_apply_cmd_args(char *cmd_buffer, int *ptr_arg)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;

	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		if (arg_index == 1) {
			if (strcmp(p, "EDGE") == 0)
				*ptr_arg = APPLY_PARAM_EDGE;
			else if (strcmp(p, "SHARPEN") == 0)
				*ptr_arg = APPLY_PARAM_SHARPEN;
			else if (strcmp(p, "BLUR") == 0)
				*ptr_arg = APPLY_PARAM_BLUR;
			else if (strcmp(p, "GAUSSIAN_BLUR") == 0)
				*ptr_arg = APPLY_PARAM_GAUSSIAN_BLUR;
			else
				*ptr_arg = APPLY_PARAM_BAD;
		}

		++arg_index;
		p = strtok(NULL, " ");
	}
}

void get_rotate_cmd_args(char *cmd_buffer, int *ptr_angle)
{
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;

	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		if (arg_index == 1)
			*ptr_angle = atoi(p);

		++arg_index;
		p = strtok(NULL, " ");
	}
}
