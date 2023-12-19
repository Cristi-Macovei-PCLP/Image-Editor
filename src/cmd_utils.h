#include "structs.h"

int check_command(char *cmd_buffer, char *cmd_name);

void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename);

void get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel);

void get_histogram_cmd_args(char *cmd_buffer, int *ptr_x, int *ptr_y);

void get_save_cmd_args(char *cmd_buffer, char **ptr_filename);

void get_apply_cmd_args(char *cmd_buffer, int *ptr_arg);

void get_rotate_cmd_args(char *cmd_buffer, int *ptr_angle);