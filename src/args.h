#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>


struct Args {
	bool directory, recursive;
	bool force, overwrite;
	bool use_spaces;
	int tab_width;
};

void initialize_args(struct Args *args);

bool isarg(const char *arg, const char *long_name, const char *short_name);

#endif