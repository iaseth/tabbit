#include "args.h"

#include <string.h>



void initialize_args(struct Args *args) {
	args->force = false;
	args->directory = false;
	args->recursive = false;
	args->overwrite = false;
	args->use_spaces = false;
	args->tab_width = 4;
}

bool isarg(const char *arg, const char *long_name, const char *short_name) {
	if (strcmp(arg, long_name) == 0 || strcmp(arg, short_name) == 0) {
		return true;
	}
	return false;
}