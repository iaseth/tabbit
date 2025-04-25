#ifndef FSUTILS_H
#define FSUTILS_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_FILE_SIZE (1024 * 1024)

bool has_known_extension(const char *filename);
bool is_hidden(const char *name);
bool is_skipped_dir(const char *name);
bool is_dir(const char *path);

bool file_too_large(const char *path);
int get_file_size(const char *filepath);

#endif