#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>


struct Args {
	bool directory, recursive;
	bool force, replace;
	bool use_spaces;
	int tab_width;
};

#endif