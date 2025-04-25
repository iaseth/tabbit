#ifndef TABBIT_H
#define TABBIT_H

#include "args.h"

#define MAX_PATH 4096
#define MAX_LINE 8192

bool process_file(const char *path, struct Args args);
void traverse_dir(const char *path, struct Args args);

#endif