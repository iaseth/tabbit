#include "tabbit.h"

#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "fsutils.h"
#include "utils.h"



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
	} else if (args.update) {
		if (changed) {
			printf("Needs update: '%s'\n", path);
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
			if (has_known_extension(entry->d_name) || args.force) {
				if (args.list) {
					printf("File: '%s'\n", full_path);
				} else if (args.force || !file_too_large(full_path)) {
					process_file(full_path, args);
				}
			}
		}
	}

	closedir(dir);
}
