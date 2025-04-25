#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "args.h"
#include "fsutils.h"
#include "tabbit.h"
#include "version.h"

#define VERSION "0.1.0"



const char* COMPILE_INFO = "Compiled on " __DATE__ " at " __TIME__;

void print_help() {
	puts("Usage: tabbit [OPTIONS] path...\n"
		"Options:\n"
		"  -f, --force         Process files over 1MB or unknown extensions\n"
		"  -d, --directory     Allow processing of directories\n"
		"  -r, --recursive     Recurse into subdirectories\n"

		"\n"
		"  -t, --tab-width N   Set tab width (default: 4)\n"
		"  -s, --spaces        Use spaces instead of tabs\n"

		"\n"
		"  -o, --overwrite     Overwrite original files with formatted version\n"
		"  -l, --list          Lists matching files\n"
		"  -u, --update        Lists files needing updates\n"

		"\n"
		"  -a, --args          Print args\n"
		"  -h, --help          Show help message\n"
		"  -v, --version       Show version info");
}

void print_version() {
	printf("%s - %s - %s\n", APP_NAME, VERSION_FULL_STR, COMPILE_INFO);
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

					case 'l': args.list = true; break;
					case 'o': args.overwrite = true; break;
					case 'u': args.update = true; break;

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
		} else if (isarg(arg, "-l", "--list")){
			args.list = true;
		} else if (isarg(arg, "-o", "--overwrite")){
			args.overwrite = true;
		} else if (isarg(arg, "-u", "--update")){
			args.update = true;
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
