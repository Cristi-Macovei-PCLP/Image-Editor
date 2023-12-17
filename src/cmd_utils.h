#include "structs.h"

int check_command(char *cmd_buffer, char *cmd_name);

void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename);

void get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel);