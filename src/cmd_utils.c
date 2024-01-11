// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "structs.h"

// aceasta functie verifica daca o comanda se incadreaza intr-un anumit tip
// tipul de verificat este precizat prin parametrul cmd_name
// aceasta functie returneaza 1 daca comanda este de tipul cautat
// si 0 daca nu
int check_command(char *cmd_buffer, char *cmd_name)
{
	if (strlen(cmd_buffer) < strlen(cmd_name))
		return 0;

	// i know strcmp is a thing, i just don't like it
	for (int i = 0; i < (int)strlen(cmd_name); ++i)
		if (cmd_buffer[i] != cmd_name[i])
			return 0;

	// daca mai are caractere si dupa lungimea tipului de comanda cautat
	// functia returneaza 0
	if (isalnum(cmd_buffer[strlen(cmd_name)]))
		return 0;

	return 1;
}

// aceasta functie citeste argumentul comenzii load si il salveaza
// intr-o locatie trimisa prin parametrul ptr_filename
void get_load_cmd_arg(char *cmd_buffer, char **ptr_filename)
{
	// impart comanda in cuvinte folosind strtok
	char *p = strtok(cmd_buffer, " ");
	int arg_index = 0;
	while (p) {
		if (strlen(p) == 0) {
			p = strtok(NULL, " ");
			continue;
		}

		// numele fisierului trebuie sa fie primul argument al comenzii
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

// aceasta functie verifica daca un sir de caractere reprezinta un numar
// functia returneaza 1 daca da si 0 daca nu
int is_numerical_string(char *string)
{
	int n = strlen(string);

	// primul caracter poate fi '-' sau o cifra
	if (string[0] != '-' && !isdigit(string[0]))
		return 0;

	// urmatoarele caractere trebuie sa fie cifre
	for (int i = 1; i < n; ++i)
		if (!isdigit(string[i]))
			return 0;

	return 1;
}

// aceasta functie citeste argumentele comenzii select
// si returneaza o valoare care arata daca valorile citite sunt corecte
// valoarea returnata:
// 0 - coordonatele citite sunt incorecte (in afara imaginii)
// 1 - coordonatele citite sunt corecte
// 2 - comanda invalida (nu a primit 4 argumente numerice)
int get_select_cmd_args(char *cmd_buffer, selection_t *ptr_sel,
						image_file_t *img_file)
{
	// impart comanda in cuvinte cu strtok
	// poate avea un singur argument "ALL"
	// sau 4 argumente numerice (x1, y1, x2, y2 in aceasta ordine)
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

		// primul argument poate fi ori "ALL" ori numeric (x1)
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

	// daca nu avem 4 argumente numerice returneaza 2
	if (arg_index <= 4)
		return 2;

	// verifica daca cele 4 valori citite sunt egale
	// daca sunt egale x1=x2 sau y1=y2, valorile sunt incorecte
	ptr_sel->is_all = 0;
	if (ptr_sel->top_left.line == ptr_sel->bot_right.line ||
		ptr_sel->top_left.col == ptr_sel->bot_right.col) {
		return 0;
	}

	// verifica daca x1 < x2
	// daca nu e adevarat, se face swap intre x1 si x2
	// la fel la y
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

// aceasta citeste argumentele pentru histograma
// returneaza 1 daca argumentele sunt valide si 0 daca nu
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

// aceasta functie citeste argumentele comenzii save
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

// aceasta functie citeste valoarea argumentului comenzii APPLY
// aceasta valoare poate fi EDGE, SHARPEN, BLUR, GAUSSIAN_BLUR
// orice alta valoare este incorecta
// daca comanda APPLY nu are parametru, se pastreaza valoarea
// de la adresa ptr_arg (care este initializata cu PARAM_NONEXISTENT)
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

// aceasta functie citeste argumentul comenzii rotate
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
