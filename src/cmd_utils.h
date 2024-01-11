// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

#ifndef GUARD_CMD_UTILS_H
#define GUARD_CMD_UTILS_H

#include "structs.h"

int check_command(char *cmd_buffer, char *cmd_name);

void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename);

int get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel,
						image_file_t *img_file);

int get_histogram_cmd_args(char *cmd_buffer, int *ptr_x, int *ptr_y);

void get_save_cmd_args(char *cmd_buffer, char **ptr_filename,
					   int *ptr_is_ascii);

void get_apply_cmd_args(char *cmd_buffer, int *ptr_arg);

void get_rotate_cmd_args(char *cmd_buffer, int *ptr_angle);

#endif
