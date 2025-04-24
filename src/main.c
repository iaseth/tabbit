#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

#define VERSION "0.1.0"
#define MAX_PATH 4096
#define MAX_LINE 8192
#define READ_BUFFER_SIZE 8192
#define MAX_FILE_SIZE (1024 * 1024)

const char *KNOWN_EXTENSIONS[] = {
	".c", ".cpp", ".h", ".hpp",
	".java", ".cs",
	".py", ".rb",
	".go", ".rs",

	".html", ".xhtml", ".xml",
	".css", ".scss", ".sass",
	".js", ".ts", ".jsx", ".tsx",
	".svelte", ".vue",

	NULL
};

const char *SKIP_DIR_NAMES[] = {
	"node_modules", "build", "dist", "out", "output",
	"debug", "release", "bin", "obj",
	"venv", ".venv", "__pycache__",
	".git", ".hg", ".svn", ".idea", ".vscode", ".DS_Store",
	"target", "env", ".env",
	"coverage", "logs", "tmp", "temp",
	NULL
};

bool has_known_extension(const char *filename) {
	const char *ext = strrchr(filename, '.');
	if (!ext) return false;
	for (int i = 0; KNOWN_EXTENSIONS[i]; i++) {
		if (strcmp(ext, KNOWN_EXTENSIONS[i]) == 0)
			return true;
	}
	return false;
}

bool is_hidden(const char *name) {
	return name[0] == '.';
}

bool is_skipped_dir(const char *name) {
	for (int i = 0; SKIP_DIR_NAMES[i]; i++) {
		if (strcmp(name, SKIP_DIR_NAMES[i]) == 0)
			return true;
	}
	return false;
}

bool is_dir(const char *path) {
	struct stat s;
	return stat(path, &s) == 0 && S_ISDIR(s.st_mode);
}

bool file_too_large(const char *path) {
	struct stat s;
	return stat(path, &s) == 0 && s.st_size > MAX_FILE_SIZE;
}

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
		"  -x, --replace       Replace original files with formatted version\n"
		"  -t, --tab-width N   Set tab width (default: 4)\n"
		"  -s, --spaces        Use spaces instead of tabs\n"
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

bool process_file(const char *path, bool replace, int tab_width, bool use_spaces) {
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
		char *indented = indent_line(line, tab_width, use_spaces);
		if (strcmp(line, indented) != 0)
			changed = true;
		fputs(indented, out);
	}

	fclose(in);
	fclose(out);

	if (!replace) {
		print_file(tmp_path);
	}

	if (changed && replace) {
		remove(path);
		rename(tmp_path, path);
	} else {
		remove(tmp_path);
	}

	return true;
}

void traverse_dir(const char *path, bool recursive, bool force, bool replace, int tab_width, bool use_spaces) {
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

		if (entry->d_type == DT_DIR && recursive) {
			traverse_dir(full_path, recursive, force, replace, tab_width, use_spaces);
		} else if (entry->d_type == DT_REG) {
			if ((has_known_extension(entry->d_name) || force) &&
				(force || !file_too_large(full_path))) {
				process_file(full_path, replace, tab_width, use_spaces);
			}
		}
	}

	closedir(dir);
}

int main(int argc, char **argv) {
	bool force = false, directory = false, recursive = false;
	bool replace = false, use_spaces = false;
	int tab_width = 4;

	const char *paths[argc];
	int path_count = 0;

	// First pass: parse flags
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0)
			force = true;
		else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--directory") == 0)
			directory = true;
		else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0)
			recursive = true;
		else if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--replace") == 0)
			replace = true;
		else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--spaces") == 0)
			use_spaces = true;
		else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tab-width") == 0) && i + 1 < argc) {
			tab_width = atoi(argv[++i]);
			if (tab_width <= 0) tab_width = 4;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			return 0;
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
			print_version();
			return 0;
		} else {
			paths[path_count++] = argv[i]; // not a flag => path
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
			if (directory) {
				traverse_dir(path, recursive, force, replace, tab_width, use_spaces);
			}
		} else {
			if (force || !file_too_large(path)) {
				process_file(path, replace, tab_width, use_spaces);
			}
		}
	}

	return 0;
}
