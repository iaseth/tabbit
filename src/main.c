#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "args.h"
#include "fsutils.h"

#define VERSION "0.1.0"
#define MAX_PATH 4096
#define MAX_LINE 8192
#define READ_BUFFER_SIZE 8192



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

void print_help() {
	puts("Usage: tabbit [OPTIONS] path...\n"
		"Options:\n"
		"  -f, --force         Process files over 1MB or unknown extensions\n"
		"  -d, --directory     Allow processing of directories\n"
		"  -r, --recursive     Recurse into subdirectories\n"
		"  -o, --overwrite     Overwrite original files with formatted version\n"
		"  -t, --tab-width N   Set tab width (default: 4)\n"
		"  -s, --spaces        Use spaces instead of tabs\n"

		"\n"
		"  -a, --args          Print args\n"

		"\n"
		"  -h, --help          Show help message\n"
		"  -v, --version       Show version info");
}

void print_version() {
	printf("tabbit %s\n", VERSION);
}

char *indent_line(const char *line, int tab_width, bool use_spaces) {
	static char buf[MAX_LINE];
	int indent_units = 0, i = 0;

	// Count both tabs and spaces as indentation units
	while (line[i] == ' ' || line[i] == '\t') {
		if (line[i] == '\t') {
			indent_units++;
			i++;
		} else {
			int spaces = 0;
			while (line[i] == ' ') {
				spaces++;
				i++;
			}
			indent_units += spaces / tab_width;
		}
	}

	const char *start = line + i;
	int j = 0;

	// Write new indentation
	for (int u = 0; u < indent_units; u++) {
		if (use_spaces) {
			for (int s = 0; s < tab_width; s++)
				buf[j++] = ' ';
		} else {
			buf[j++] = '\t';
		}
	}

	// Copy rest of line
	while (*start && *start != '\n')
		buf[j++] = *start++;

	// Trim trailing spaces/tabs
	while (j > 0 && (buf[j - 1] == ' ' || buf[j - 1] == '\t'))
		j--;

	if (*start == '\n') buf[j++] = '\n';
	buf[j] = '\0';

	return buf;
}

bool process_file(const char *path, struct Args args) {
	FILE *in = fopen(path, "r");
	if (!in) return false;

	char tmp_path[MAX_PATH];
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
	FILE *out = fopen(tmp_path, "w");
	if (!out) {
		fclose(in);
		return false;
	}

	bool changed = false;
	char line[MAX_LINE];

	while (fgets(line, sizeof(line), in)) {
		char *indented = indent_line(line, args.tab_width, args.use_spaces);
		if (strcmp(line, indented) != 0)
			changed = true;
		fputs(indented, out);
	}

	fclose(in);
	fclose(out);

	if (args.overwrite) {
		if (changed) {
			remove(path);
			rename(tmp_path, path);
			printf("Updated: '%s'\n", path);
		}
	} else {
		print_file(tmp_path);
	}

	remove(tmp_path);
	return true;
}

void traverse_dir(const char *path, struct Args args) {
	DIR *dir = opendir(path);
	if (!dir) return;

	struct dirent *entry;
	char full_path[MAX_PATH];

	while ((entry = readdir(dir))) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		if (is_hidden(entry->d_name) || is_skipped_dir(entry->d_name))
			continue;

		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		if (entry->d_type == DT_DIR && args.recursive) {
			traverse_dir(full_path, args);
		} else if (entry->d_type == DT_REG) {
			if ((has_known_extension(entry->d_name) || args.force) &&
				(args.force || !file_too_large(full_path))) {
				process_file(full_path, args);
			}
		}
	}

	closedir(dir);
}

int main(int argc, char **argv) {
	struct Args args;
	initialize_args(&args);

	const char *paths[argc];
	int path_count = 0;

	// First pass: parse flags
	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		if (strlen(arg) > 2 && arg[0] == '-' && arg[1] != '-') {
			const char *pch = arg + 1;
			while (*pch) {
				switch (*pch) {
					case 'f': args.force = true; break;
					case 'd': args.directory = true; break;
					case 'r': args.recursive = true; break;
					case 'o': args.overwrite = true; break;
					case 's': args.use_spaces = true; break;
					default:
						printf("Invalid option: '%c'\n", *pch);
						return 1;
				}
				pch++;
			}
		} else if (isarg(arg, "-a", "--args")) {
			print_args(&args);
			return 0;
		} else if (isarg(arg, "-f", "--force")) {
			args.force = true;
		} else if (isarg(arg, "-d", "--directory")) {
			args.directory = true;
		} else if (isarg(arg, "-r", "--recursive")) {
			args.recursive = true;
		} else if (isarg(arg, "-o", "--overwrite")){
			args.overwrite = true;
		} else if (isarg(arg, "-s", "--spaces")) {
			args.use_spaces = true;
		} else if (isarg(arg, "-t", "--tab-width") && i + 1 < argc) {
			args.tab_width = atoi(argv[++i]);
			if (args.tab_width <= 0) args.tab_width = 4;
		} else if (isarg(arg, "-h", "--help")) {
			print_help();
			return 0;
		} else if (isarg(arg, "-v", "--version")) {
			print_version();
			return 0;
		} else {
			paths[path_count++] = arg; // not a flag => path
		}
	}

	if (path_count == 0) {
		print_help();
		return 1;
	}

	// Second pass: process paths
	for (int i = 0; i < path_count; i++) {
		const char *path = paths[i];
		if (is_dir(path)) {
			if (args.directory) {
				traverse_dir(path, args);
			}
		} else {
			if (args.force || !file_too_large(path)) {
				process_file(path, args);
			}
		}
	}

	return 0;
}
