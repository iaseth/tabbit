#include "args.h"

#include <stdio.h>
#include <string.h>



void initialize_args(struct Args *args) {
	args->force = false;
	args->directory = false;
	args->recursive = false;

	args->overwrite = false;
	args->list = false;
	args->update = false;

	args->use_spaces = false;
	args->tab_width = 4;
}


void print_boolean_arg(char *name, int value) {
	printf("\t %10s => ", name);

	if (value == 0) printf("false\n");
	else if (value == 1) printf("true\n");
	else printf("%d\n", value);
}

void print_args(struct Args *args) {
	print_boolean_arg("force", args->force);
	print_boolean_arg("directory", args->directory);
	print_boolean_arg("recursive", args->recursive);

	print_boolean_arg("overwrite", args->overwrite);
	print_boolean_arg("list", args->overwrite);
	print_boolean_arg("update", args->update);

	print_boolean_arg("use_spaces", args->use_spaces);
	print_boolean_arg("tab_width", args->tab_width);
}


bool isarg(const char *arg, const char *long_name, const char *short_name) {
	if (strcmp(arg, long_name) == 0 || strcmp(arg, short_name) == 0) {
		return true;
	}
	return false;
}