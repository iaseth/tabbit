#include "utils.h"

#include <stdio.h>



bool print_file(const char *path) {
	FILE *file = fopen(path, "r");
	if (file == NULL) return true;

	char buff[READ_BUFFER_SIZE];
	size_t n;
	while ((n = fread(buff, sizeof(char), sizeof(buff) - 1, file)) > 0) {
		buff[n] = '\0';
		printf("%s", buff);
	}

	fclose(file);
	return false;
}
