
# tabbit

> A fast and configurable CLI tool to normalize indentation in source code files.

`tabbit` automatically converts indentation to either tabs or spaces, trims trailing whitespace, and optionally replaces the original file. It is useful for codebases with inconsistent formatting or for enforcing personal/team indentation standards.

---

## 🚀 Features

- Converts indentation in source files to **tabs** or **spaces**
- Trims trailing whitespace from all lines
- Supports **custom tab width** (`--tab-width`)
- Handles individual files, directories, and nested subdirectories
- Skips unnecessary processing when files are already correctly indented
- Automatically ignores common build, version control, and environment folders
- Preserves file size safety limits unless `--force` is used
- Works on common file types like `.c`, `.py`, `.js`, `.cpp`, etc.

---

## 🛠 Usage

```bash
tabbit [OPTIONS] path...
```

### Paths
- Explicit file paths are always processed.
- Directories are only processed with `--directory` (`-d`)
- Subdirectories are only traversed with `--recursive` (`-r`)

---

## ⚙️ Options

| Flag                | Description                                              |
|---------------------|----------------------------------------------------------|
| `-f`, `--force`      | Process unknown file types or files >1MB                 |
| `-d`, `--directory`  | Enable directory processing                              |
| `-r`, `--recursive`  | Recurse into subdirectories                              |
| `-x`, `--replace`    | Replace original files with the formatted output         |
| `-t`, `--tab-width`  | Number of spaces that equal one tab (default: `4`)       |
| `-s`, `--spaces`     | Use spaces instead of tabs for indentation               |
| `-h`, `--help`       | Show help message                                        |
| `-v`, `--version`    | Show version info                                        |

---

## 🧠 Smart Behavior

- Ignores hidden files and directories (`.git`, `.vscode`, etc.)
- Skips known build-related or virtual environment folders:
  ```
  node_modules, build, dist, debug, release, bin, obj,
  .git, .idea, .venv, venv, __pycache__, target, etc.
  ```
- Only rewrites files if formatting changes are required
- Tab width and output style are fully configurable

---

## 📦 Build:

```bash
mkdir build && cd build
cmake ..
make
```

---

## 🧪 Example

```bash
# Convert a Python file to tabs, replacing the original
tabbit -x main.py

# Convert all source files in `src/` to tabs
tabbit -d -r -x src/
```
