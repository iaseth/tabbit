#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>


struct Args {
	bool force;
	bool directory, recursive;
	bool overwrite, list;

	bool use_spaces;
	int tab_width;
};

void initialize_args(struct Args *args);
void print_args(struct Args *args);

bool isarg(const char *arg, const char *long_name, const char *short_name);

#endif