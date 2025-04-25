#include "fsutils.h"

#include <limits.h>
#include <string.h>
#include <sys/stat.h>



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

int get_file_size(const char *filepath) {
	struct stat st;
	if (stat(filepath, &st) != 0) {
		return 0; // Error occurred
	}

	if (st.st_size > INT_MAX) {
		return 0; // Too large to fit in int
	}

	return (int)st.st_size;
}
