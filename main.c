//Copyright ALDICA MARIA MIRUNA 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//Struct which contains information about pixels
typedef struct {
	double ***pixels;
	int rows, cols, x1, x2, y1, y2, max, nr_elements;
} image;
//Function to create an image
void create_image(image *current, int rows, int cols, int nr_elements, int max)
{
	current->rows = rows;
	current->cols = cols;
	current->nr_elements = nr_elements;
	current->max = max;

	//alloc the lines
	current->pixels = (double ***)calloc(nr_elements, sizeof(double **));
	for (int i = 0; i < nr_elements; i++) {
		//alloc every line
		current->pixels[i] = (double **)calloc(rows, sizeof(double *));
		for (int j = 0; j < rows; j++)
			current->pixels[i][j] = (double *)calloc(cols, sizeof(double));
	}
	//Coordinates of the full size photo
	current->x1 = 0;
	current->x2 = cols;
	current->y1 = 0;
	current->y2 = rows;
}

//Function to free the memory allocated
void free_image(image current)
{
	for (int i = 0; i < current.nr_elements; i++) {
		for (int j = 0; j < current.rows; j++)
			free(current.pixels[i][j]);
		free(current.pixels[i]);
	}
	free(current.pixels);
}

//Function to read the binary elements
void read_elements_binary(image *current, FILE *input)
{
	for (int i = 0; i < current->rows; i++)
		for (int j = 0; j < current->cols; j++)
			for (int k = 0; k < current->nr_elements; k++) {
				unsigned char x;
				fread(&x, 1, 1, input);
				current->pixels[k][i][j] = x;
			}
}

//Function to read the ascii elements
void read_elements_text(image *current, FILE *input)
{
	for (int i = 0; i < current->rows; i++)
		for (int j = 0; j < current->cols; j++)
			for (int k = 0; k < current->nr_elements; k++)
				fscanf(input, "%lf", &current->pixels[k][i][j]);
}

//Function to load an image
void load_image(image *current, int *loaded)
{
	if (*loaded)
		free_image(*current);

	*loaded = 0;

	char f[101];
	scanf("%s", f);

	FILE *input = fopen(f, "rt");

	if (!input) {
		printf("Failed to load %s\n", f);
		return;
	}

	fseek(input, 1, SEEK_SET);

	int magic_word, rows, cols, max;
	//What type of file is loaded by checking the magic word
	fscanf(input, "%d%d%d%d", &magic_word, &cols, &rows, &max);

	create_image(current, rows, cols, magic_word % 3 == 0 ? 3 : 1, max);

	if (magic_word > 4) {
		int cursor = ftell(input);
		fclose(input);
		input = fopen(f, "rb");
		fseek(input, cursor + 1, SEEK_SET);

		read_elements_binary(current, input);
	} else {
		read_elements_text(current, input);
	}

	fclose(input);

	printf("Loaded %s\n", f);
	*loaded = 1;
}

//Function to print the binary images
void print_elements_binary(image current, FILE *output)
{
	for (int i = 0; i < current.rows; i++)
		for (int j = 0; j < current.cols; j++)
			for (int k = 0; k < current.nr_elements; k++) {
				unsigned char x = round(current.pixels[k][i][j]);
				fwrite(&x, 1, 1, output);
			}
}

//Function to print the ASCII images
void print_elements_text(image current, FILE *output)
{
	for (int i = 0; i < current.rows; i++) {
		for (int j = 0; j < current.cols; j++)
			for (int k = 0; k < current.nr_elements; k++) {
				unsigned char x = round(current.pixels[k][i][j]);
				fprintf(output, "%hhu ", x);
			}
		fprintf(output, "\n");
	}
}

//Function to save the image into a file
void save_image(image current)
{
	char f[101], *name;
	fgets(f, 101, stdin);
	int ascii;
	if (strstr(f, "ascii"))
		ascii = 1;
	else
		ascii = 0;

	name = strtok(f, "\n ");

	int magic_word = (current.nr_elements == 3 ? 3 : 2) + 3 * !ascii;

	FILE *output = fopen(name, "w");
	if (!output)
		return;
	//See what to print on the file open usong the magicword
	fprintf(output, "P%d\n%d %d\n%d\n", magic_word,
			current.cols, current.rows, current.max);

	if (ascii)
		print_elements_text(current, output);
	else
		print_elements_binary(current, output);

	fclose(output);
	printf("Saved %s\n", name);
}

//Function to swap two int pointers
void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

//Function to select a part of an image
void select_image(image *current)
{
	char nr[101];
	fgets(nr, 100, stdin);
	char *t = strtok(nr, "\n ");
	if (!t) {
		printf("Invalid command\n");
		return;
	}
	//Coordinates if we select all image
	if (strcmp(t, "ALL") == 0) {
		current->x1 = 0;
		current->x2 = current->cols;
		current->y1 = 0;
		current->y2 = current->rows;
		printf("Selected ALL\n");
		return;
	}
	int x1, x2, y1, y2;
	x1 = atoi(t);
	if (x1 == 0 && strcmp(t, "0")) {
		printf("Invalid command\n");
		return;
	} else if (x1 < 0 || x1 > current->cols) {
		printf("Invalid set of coordinates\n");
		return;
	}

	t = strtok(NULL, "\n ");
	if (!t) {
		printf("Invalid command\n");
		return;
	}
	y1 = atoi(t);
	if (y1 == 0 && strcmp(t, "0")) {
		printf("Invalid command\n");
		return;
	} else if (y1 < 0 || y1 > current->rows) {
		printf("Invalid set of coordinates\n");
		return;
	}

	t = strtok(NULL, "\n ");
	if (!t) {
		printf("Invalid command\n");
		return;
	}
	x2 = atoi(t);
	if (x2 == 0 && strcmp(t, "0")) {
		printf("Invalid command\n");
		return;
	} else if (x2 < 0 || x2 > current->cols || x1 == x2) {
		printf("Invalid set of coordinates\n");
		return;
	}

	t = strtok(NULL, "\n ");
	if (!t) {
		printf("Invalid command\n");
		return;
	}
	y2 = atoi(t);
	if (y2 == 0 && strcmp(t, "0")) {
		printf("Invalid command\n");
		return;
	} else if (y2 < 0 || y2 > current->rows || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	//Check is the order or coordinates is valid
	if (y1 > y2)
		swap(&y1, &y2);
	if (x1 > x2)
		swap(&x1, &x2);

	current->x1 = x1;
	current->x2 = x2;
	current->y1 = y1;
	current->y2 = y2;

	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

//Function to check if an image was loaded
int check_loaded(int loaded)
{
	if (!loaded) {
		char line[101];
		fgets(line, 100, stdin);
		printf("No image loaded\n");
	}
	return loaded;
}

//Function to crop a part of an image
void crop_image(image *current)
{
	image new;
	//We create a copy of the old image
	create_image(&new, current->y2 - current->y1, current->x2 - current->x1,
				 current->nr_elements, current->max);

	for (int i = 0; i < new.nr_elements; i++)
		for (int j = 0; j < new.rows; j++)
			for (int k = 0; k < new.cols; k++)
				new.pixels[i][j][k] =
				current->pixels[i][j + current->y1][k + current->x1];

	free_image(*current);

	*current = new;

	printf("Image cropped\n");
}

//Function to change once a pixel
void rotate_once_all(image *current)
{
	image copy_;
	create_image(&copy_, current->cols,
				 current->rows,
				 current->nr_elements, current->max);
	for (int i = 0; i < copy_.nr_elements; i++)
		for (int j = 0; j < copy_.rows; j++)
			for (int k = 0; k < copy_.cols; k++)
				copy_.pixels[i][j][k] =
				current->pixels[i][current->rows - 1 - k][j];
	free_image(*current);
	*current = copy_;
}

//Function to get the transpose of the image
void rotate_once(image *current)
{
	image copy_;
	create_image(&copy_, current->y2 - current->y1,
				 current->x2 - current->x1,
				 current->nr_elements, current->max);
	for (int i = 0; i < copy_.nr_elements; i++)
		for (int j = 0; j < copy_.rows; j++)
			for (int k = 0; k < copy_.cols; k++)
				copy_.pixels[i][j][k] =
				current->pixels[i][current->y2 - k - 1][current->x1 + j];
	for (int i = 0; i < copy_.nr_elements; i++)
		for (int j = 0; j < copy_.rows; j++)
			for (int k = 0; k < copy_.cols; k++)
				current->pixels[i][current->y1 + j][current->x1 + k] =
				copy_.pixels[i][j][k];
	free_image(copy_);
}

//Function to rotate the fully selected photo
void rotate_image(image *current)
{
	char nr[101];
	fgets(nr, 100, stdin);

	char *t = strtok(nr, "\n ");
	int angle = atoi(t);

	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	} else if (current->y2 - current->y1 != current->x2 - current->x1) {
		if (current->x1 != 0 || current->y1 != 0 ||
			current->x2 != current->cols || current->y2 != current->rows)
			printf("The selection must be square\n");
	}

	if (angle < 0)
		angle += 360;
	if (current->y2 - current->y1 == current->x2 - current->x1) {
		for (int i = 0; i < angle; i += 90)
			rotate_once(current);
	} else {
		for (int i = 0; i < angle; i += 90)
			rotate_once_all(current);
	}
	printf("Rotated %d\n", atoi(t));
}

//Function to apply different efects on images
double compute(image *current, int element, int line, int col, char *name)
{
	double val;
	if (strcmp(name, "EDGE") == 0)
		val = 8.0 * current->pixels[element][line][col] -
		current->pixels[element][line - 1][col - 1] -
		current->pixels[element][line - 1][col] -
		current->pixels[element][line - 1][col + 1] -
		current->pixels[element][line][col - 1] -
		current->pixels[element][line][col + 1] -
		current->pixels[element][line + 1][col - 1] -
		current->pixels[element][line + 1][col] -
		current->pixels[element][line + 1][col + 1];
	else if (strcmp(name, "SHARPEN") == 0)
		val = 5.0 * current->pixels[element][line][col] -
		current->pixels[element][line - 1][col] -
		current->pixels[element][line + 1][col] -
		current->pixels[element][line][col - 1] -
		current->pixels[element][line][col + 1];
	else if (strcmp(name, "BLUR") == 0)
		val = 1.0 / 9 *
		(current->pixels[element][line - 1][col - 1] +
		 current->pixels[element][line - 1][col] +
		 current->pixels[element][line - 1][col + 1] +
		 current->pixels[element][line][col - 1] +
		 current->pixels[element][line][col] +
		 current->pixels[element][line][col + 1] +
		 current->pixels[element][line + 1][col - 1] +
		 current->pixels[element][line + 1][col] +
		 current->pixels[element][line + 1][col + 1]);
	else if (strcmp(name, "GAUSSIAN_BLUR") == 0)
		val = 1.0 / 16 *
		(current->pixels[element][line - 1][col - 1] +
		 2 * current->pixels[element][line - 1][col] +
		 current->pixels[element][line - 1][col + 1] +
		 2 * current->pixels[element][line][col - 1] +
		 4 * current->pixels[element][line][col] +
		 2 * current->pixels[element][line][col + 1] +
		 current->pixels[element][line + 1][col - 1] +
		 2 * current->pixels[element][line + 1][col] +
		 current->pixels[element][line + 1][col + 1]);

	if (val < 0)
		val = 0;
	else if (val > current->max)
		val = current->max;

	return val;
}

void apply_image(image *current)
{
	char filter[101];
	fgets(filter, 100, stdin);

	char *name = strtok(filter, "\n ");

	if (!name) {
		printf("Invalid command\n");
		return;
	}

	if (strcmp(name, "EDGE") && strcmp(name, "SHARPEN") &&
		strcmp(name, "BLUR") && strcmp(name, "GAUSSIAN_BLUR")) {
		printf("APPLY parameter invalid\n");
		return;
	}

	if (current->nr_elements != 3) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	image temp;

	create_image(&temp, current->y2 - current->y1, current->x2 - current->x1,
				 current->nr_elements, current->max);

	for (int i = 0; i < current->nr_elements; i++)
		for (int j = current->y1; j < current->y2; j++)
			for (int k = current->x1; k < current->x2; k++)
				if (j == 0 || k == 0 ||
					j == current->rows - 1 || k == current->cols - 1)
					temp.pixels[i][j - current->y1][k - current->x1] =
					current->pixels[i][j][k];
				else
					temp.pixels[i][j - current->y1][k - current->x1] =
					compute(current, i, j, k, name);

	for (int i = 0; i < current->nr_elements; i++)
		for (int j = current->y1; j < current->y2; j++)
			for (int k = current->x1; k < current->x2; k++)
				current->pixels[i][j][k] =
				temp.pixels[i][j - current->y1][k - current->x1];

	free_image(temp);

	printf("APPLY %s done\n", name);
}

int main(void)
{
	char task[101];

	int loaded = 0;
	image current;
	current.pixels = NULL;

	scanf("%s", task);

	//Apply different filters
	while (1) {
		if (strcmp(task, "LOAD") == 0) {
			load_image(&current, &loaded);
		} else if (strcmp(task, "SAVE") == 0) {
			if (check_loaded(loaded))
				save_image(current);
		} else if (strcmp(task, "EXIT") == 0) {
			check_loaded(loaded);
			break;
		} else if (strcmp(task, "SELECT") == 0) {
			if (check_loaded(loaded))
				select_image(&current);
		} else if (strcmp(task, "CROP") == 0) {
			if (check_loaded(loaded))
				crop_image(&current);
		} else if (strcmp(task, "ROTATE") == 0) {
			if (check_loaded(loaded))
				rotate_image(&current);
		} else if (strcmp(task, "APPLY") == 0) {
			if (check_loaded(loaded))
				apply_image(&current);
		} else {
			printf("Invalid command\n");
			char line[101];
			fgets(line, 100, stdin);
		}

		scanf("%s", task);
	}

	if (loaded)
		free_image(current);

	return 0;
}
