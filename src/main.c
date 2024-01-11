// Macovei Nicolae-Cristian
// Anul I, grupa 312CAb

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

// aceasta functie citeste o comanda de la tastatura
// si o salveaza intr-un buffer
void read_command(char *cmd_buffer)
{
	fgets(cmd_buffer, CMD_BUFFER_SIZE * sizeof(char), stdin);

	// sterge caracterul '\n' de la finalul comenzii
	if (cmd_buffer[strlen(cmd_buffer) - 1] == '\n')
		cmd_buffer[strlen(cmd_buffer) - 1] = '\0';

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Command: '%s'\n", cmd_buffer);
#endif
}

// aceasta functie executa o comanda de tip 'LOAD'
void handle_load_cmd(int *ptr_has_file, image_file_t *img_file,
					 char *cmd_buffer, selection_t *ptr_sel)
{
	// daca este deja un fisier incarcat, eliberam memoria alocata acestuia
	if (*ptr_has_file) {
#ifdef MODE_DEBUG
		fprintf(stderr, "[debug] Freeing existing file\n");
#endif
		free_image_file(img_file);
	}

	// extrage numele fisierului (primul argument din comanda)
	char *filename = NULL;
	get_load_cmd_arg(cmd_buffer, &filename);

	if (!filename) {
		printf("No filename for 'LOAD' found\n");
		*ptr_has_file = 0;
		if (filename)
			free(filename);

		return;
	}

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Loading file: '%s'\n", filename);
#endif

	// deschide fisierul si citeste datele imaginii
	FILE * file = fopen(filename, "rb");

	if (!file) {
		printf("Failed to load %s\n", filename);
		*ptr_has_file = 0;
		free(filename);
		return;
	}

	img_file->filename = filename;

	*ptr_has_file = 1;
	if (!parse_image_file(file, img_file)) {
		printf("Failed to load %s\n", filename);
		*ptr_has_file = 0;

		free_image_file(img_file);
		fclose(file);

		return;
	}

	printf("Loaded %s\n", filename);

	// dupa ce imaginea a fost incarcata, se selecteaza automat toata imaginea
	ptr_sel->is_all = 1;
	ptr_sel->top_left.line = 0;
	ptr_sel->top_left.col = 0;
	ptr_sel->bot_right.line = img_file->height;
	ptr_sel->bot_right.col = img_file->width;

	fclose(file);
}

// aceasta functie executa o comanda de tip "SELECT"
void handle_select_cmd(int *ptr_has_file, image_file_t *img_file,
					   char *cmd_buffer, selection_t *ptr_sel)
{
	// daca nu este nicio imagine incarcata,
	// nu se poate executa comanda "SELECT"
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// citim valorile intr-o variabila diferita
	// pentru a putea pastra valorile vechi, in caz ca cele citite acum
	// nu sunt corecte
	selection_t new_sel;
	int is_selection_valid =
			get_select_cmd_args(cmd_buffer, &new_sel, img_file);

	if (is_selection_valid == 1) {
		*ptr_sel = new_sel;
	} else {
		if (is_selection_valid == 0)
			printf("Invalid set of coordinates\n");
		else
			printf("Invalid command\n");

		return;
	}

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Selected %d,%d --> %d,%d\n",
			current_sel.top_left.line, current_sel.top_left.col,
			current_sel.bot_right.line, current_sel.bot_right.col);
#endif

	if (ptr_sel->is_all) {
		printf("Selected ALL\n");
	} else {
		printf("Selected %d %d %d %d\n", ptr_sel->top_left.col,
			   ptr_sel->top_left.line, ptr_sel->bot_right.col,
			   ptr_sel->bot_right.line);
	}
}

// aceasta functie executa o comanda de tip "CROP"
void handle_crop_cmd(int *ptr_has_file, image_file_t *img_file,
					 selection_t *ptr_sel)
{
	// daca nu este nicio imagine incarcata, nu se poate executa comanda "CROP"
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	crop_image(img_file, ptr_sel);

	// dupa operatia de crop, se selecteaza automat toata imaginea ramasa
	ptr_sel->is_all = 1;
	ptr_sel->top_left.line = 0;
	ptr_sel->top_left.col = 0;
	ptr_sel->bot_right.line = img_file->height;
	ptr_sel->bot_right.col = img_file->width;

	printf("Image cropped\n");
}

// aceasta functie executa o comanda de tip "ROTATE"
void handle_rotate_cmd(int *ptr_has_file, image_file_t *img_file,
					   char *cmd_buffer, selection_t *ptr_sel)
{
	// extrage valoarea unghiului (primul argument din comanda)
	int angle = -1;
	get_rotate_cmd_args(cmd_buffer, &angle);

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Rotate angle = %d\n", angle);
#endif

	// verific daca unghiul face parte din valorile suportate
	if (abs(angle) != 0 && abs(angle) != 90 && abs(angle) != 180 &&
		abs(angle) != 270 && abs(angle) != 360) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// daca nu este nicio imagine incarcata, nu putem executa operatia
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// verific daca selectia cuprinde intreaga imagine
	if (ptr_sel->is_all ||
		(ptr_sel->top_left.line == 0 &&
			ptr_sel->top_left.col == 0 &&
			ptr_sel->bot_right.line == img_file->height &&
			ptr_sel->bot_right.col == img_file->width))
		rotate_all(img_file, ptr_sel, angle);
	else
		rotate_square(img_file, ptr_sel, angle);

	printf("Rotated %d\n", angle);
}

// aceasta functie executa o comanda de tip "HISTOGRAM"
void handle_histogram_cmd(int *ptr_has_file, image_file_t *img_file,
						  char *cmd_buffer)
{
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// citesc valorile pentru numarul de caractere si numarul de binuri
	// (primul si al doilea argument din comanda)
	int x, y;
	int is_valid = get_histogram_cmd_args(cmd_buffer, &x, &y);

	// verific daca valorile sunt valide
	if (is_valid == 0) {
		printf("Invalid command\n");
		return;
	}

	// histograma nu se poate calcula pentru imagini color
	if (img_file->type == IMAGE_COLOR) {
		printf("Black and white image needed\n");
		return;
	}

	print_histogram(img_file, x, y);
}

// aceasta functie executa o comanda de tip "APPLY"
void handle_apply_cmd(int *ptr_has_file, image_file_t *img_file,
					  char *cmd_buffer, selection_t *ptr_sel)
{
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// citesc valoarea tipului de efect aplicat
	int apply_param_type = APPLY_PARAM_NON_EXISTENT;
	get_apply_cmd_args(cmd_buffer, &apply_param_type);

	if (apply_param_type == APPLY_PARAM_NON_EXISTENT) {
		printf("Invalid command\n");
		return;
	}

	if (apply_param_type == APPLY_PARAM_BAD) {
		printf("APPLY parameter invalid\n");
		return;
	}

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Param type: %d\n", apply_param_type);
#endif

	// operatia "APPLY" nu se poate aplica pe imagini alb-negru
	if (img_file->type == IMAGE_GRAYSCALE) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	apply_param(img_file, ptr_sel, apply_param_type);

	if (apply_param_type == APPLY_PARAM_EDGE)
		printf("APPLY EDGE done\n");
	else if (apply_param_type == APPLY_PARAM_SHARPEN)
		printf("APPLY SHARPEN done\n");
	else if (apply_param_type == APPLY_PARAM_BLUR)
		printf("APPLY BLUR done\n");
	else if (apply_param_type == APPLY_PARAM_GAUSSIAN_BLUR)
		printf("APPLY GAUSSIAN_BLUR done\n");
}

// aceasta functie executa o comanda de tip "EQUALIZE"
void handle_equalize_cmd(int *ptr_has_file, image_file_t *img_file)
{
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// egalizarea se poate realiza doar pe imagini de tip alb-negru
	if (img_file->type != IMAGE_GRAYSCALE) {
		printf("Black and white image needed\n");
		return;
	}

	equalize_image(img_file);

	printf("Equalize done\n");
}

// aceasta functie executa o comanda de tip "SAVE"
void handle_save_cmd(int *ptr_has_file, image_file_t *img_file,
					 char *cmd_buffer)
{
	if (!*ptr_has_file) {
		printf("No image loaded\n");
		return;
	}

	// citesc argumentele comenzii
	// primul argument reprezinta numele fisierului
	// al doilea argument este optional, reprezinta tipul de fisier
	// (ascii sau binar)
	char *filename = NULL;
	int is_ascii = 0;
	get_save_cmd_args(cmd_buffer, &filename, &is_ascii);

	if (!filename) {
		printf("No filename given\n");
		return;
	}

#ifdef MODE_DEBUG
	fprintf(stderr, "[debug] Saving to %s\n", filename);
	fprintf(stderr, "[debug] Is ascii: %d\n", is_ascii);
#endif

	if (is_ascii)
		save_image_ascii(img_file, filename);
	else
		save_image_binary(img_file, filename);

	printf("Saved %s\n", filename);

	free(filename);
}

// aceasta functie se apeleaza cand programul primeste comanda "EXIT"
// elibereaza memoria folosita pentru imaginea curenta
void handle_exit_cmd(int *ptr_has_file, image_file_t *img_file)
{
#ifdef MODE_DEBUG
	printf("[debug] Exiting program with %s loaded file\n",
		   has_file ? "1" : "no");
#endif
	if (*ptr_has_file)
		free_image_file(img_file);
	else
		printf("No image loaded\n");
}

#ifdef MODE_DEBUG
void handle_show_cmd(image_file_t *img_file, selection_t *ptr_sel)
{
	fprintf(stderr, "[debug] Loaded filename: %s\n",
			current_file.filename);
	fprintf(stderr, "[debug] Image size: h = %d, w = %d\n",
			current_file.height, current_file.width);
	fprintf(stderr, "[debug] Current selection: %d, %d --> %d, %d\n",
			current_sel.top_left.line, current_sel.top_left.col,
			current_sel.bot_right.line, current_sel.bot_right.col);
}
#endif

// main.
int main(void)
{
	// sirul de caractere al fiecarei comenzi va fi stocat in acest vector
	char cmd_buffer[1 + CMD_BUFFER_SIZE];

	// aceste variabile reprezinta imaginea care se afla in memorie
	// la pasul curent
	int has_file = 0;
	image_file_t current_file = {NULL, 0, 0, 0, -1, NULL};

	// aceasta variabila reprezinta selectia curenta
	selection_t current_sel = {{0, 0}, {0, 0}, 0};

	// la fiecare pas se citeste o comanda
	// in functie de tipul comenzii, se apeleaza functia corespunzatoare
	// daca se intalneste o comanda care nu este niciunul dintre tipurile
	// cunoscute, se afiseaza mesajul "Invalid command"
	while (!feof(stdin)) {
		read_command(cmd_buffer);

		if (check_command(cmd_buffer, "LOAD")) {
			handle_load_cmd(&has_file, &current_file, cmd_buffer,
							&current_sel);
			continue;
		}

#ifdef MODE_DEBUG
		else if (check_command(cmd_buffer, "SHOW")) {
			handle_show_cmd(&current_file, &current_sel);
			continue;
		}
#endif

		else if (check_command(cmd_buffer, "SELECT")) {
			handle_select_cmd(&has_file, &current_file, cmd_buffer,
							  &current_sel);
			continue;
		}

		else if (check_command(cmd_buffer, "CROP")) {
			handle_crop_cmd(&has_file, &current_file, &current_sel);
			continue;
		}

		else if (check_command(cmd_buffer, "ROTATE")) {
			handle_rotate_cmd(&has_file, &current_file, cmd_buffer,
							  &current_sel);
			continue;
		}

		else if (check_command(cmd_buffer, "HISTOGRAM")) {
			handle_histogram_cmd(&has_file, &current_file, cmd_buffer);
			continue;
		}

		else if (check_command(cmd_buffer, "APPLY")) {
			handle_apply_cmd(&has_file, &current_file, cmd_buffer,
							 &current_sel);
			continue;
		}

		else if (check_command(cmd_buffer, "EQUALIZE")) {
			handle_equalize_cmd(&has_file, &current_file);
			continue;
		}

		else if (check_command(cmd_buffer, "SAVE")) {
			handle_save_cmd(&has_file, &current_file, cmd_buffer);
			continue;
		}

		else if (check_command(cmd_buffer, "EXIT")) {
			handle_exit_cmd(&has_file, &current_file);
			break;
		}

		printf("Invalid command\n");
	}

	return 0;
}
