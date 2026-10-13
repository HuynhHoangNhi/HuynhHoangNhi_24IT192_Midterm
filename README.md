# Simplified NetBSD `ls(1)` — Midterm Project

## 1. Project Information

- **Course:** Midterm Project
- **Operating System:** NetBSD 10.1
- **Programming Language:** C
- **Repository:** https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm

## 2. Project Description

This project implements a simplified version of the NetBSD `ls(1)` command in C. The implementation follows the subset of options specified in the course-provided NetBSD manual.

The program lists directory contents and file information. It supports selected sorting methods, recursive directory traversal, long-format output, inode numbers, block counts, file-type indicators, and filename handling options.

This is an educational implementation and is not intended to replace every behavior of the original NetBSD `ls` utility.

## 3. Project Structure

```text
ls-midterm/
├── include/
│   ├── display.h
│   ├── list.h
│   └── options.h
├── src/
│   ├── display.c
│   ├── list.c
│   ├── main.c
│   └── options.c
├── .gitignore
├── Makefile
└── README.md
```

## 4. Implementation Design

The program is divided into modules to make the code easier to understand and maintain.

- **`main.c`**: Coordinates command-line processing and file or directory listing.
- **`options.c`**: Parses command-line options.
- **`list.c`**: Reads directory entries, filters entries, obtains file metadata, and handles sorting.
- **`display.c`**: Formats and prints filenames, file information, sizes, permissions, and symbolic-link targets.
- **Header files**: Declare shared structures, functions, and option definitions.
- **`Makefile`**: Compiles the source files into the `ls` executable.
- **`.gitignore`**: Excludes generated object files, compiled executables, and the local backup file.

## 5. Compilation

Run these commands on NetBSD from the project directory:

```sh
make clean
make
```

The Makefile compiles the source files using the C compiler with warning flags enabled.

To remove generated object files and the executable:

```sh
make clean
```

## 6. Usage

List the current directory:

```sh
./ls
```

List a specified directory:

```sh
./ls test_ls_review
```

Include all entries, including `.` and `..`:

```sh
./ls -a test_ls_review
```

Include hidden entries except `.` and `..`:

```sh
./ls -A test_ls_review
```

Display detailed file information:

```sh
./ls -l test_ls_review
```

Display inode numbers:

```sh
./ls -i test_ls_review
```

List directories recursively:

```sh
./ls -R test_ls_review
```

List a directory itself instead of its contents:

```sh
./ls -d test_ls_review
```

Display file-type indicators:

```sh
./ls -F test_ls_review
```

Sort by file size:

```sh
./ls -S test_ls_review
```

Sort by modification time:

```sh
./ls -t test_ls_review
```

Display allocated block counts:

```sh
./ls -s test_ls_review
```

Display human-readable sizes:

```sh
./ls -lh test_ls_review
```

## 7. Implemented Options

The implementation supports the following option set from the supplied manual:

| Option | Description |
|---|---|
| `-A` | Include hidden entries except `.` and `..`. |
| `-a` | Include all entries, including `.` and `..`. |
| `-c` | Use status-change time for relevant time operations. |
| `-d` | List directory operands themselves. |
| `-F` | Append file-type indicators. |
| `-f` | Disable sorting and include hidden entries. |
| `-h` | Display sizes in human-readable format with `-l` or `-s`. |
| `-i` | Display inode numbers. |
| `-k` | Display block counts in kilobytes with `-s`. |
| `-l` | Display long-format file information. |
| `-n` | Display numeric user and group IDs. |
| `-q` | Replace non-printable filename characters with `?`. |
| `-R` | Recursively list subdirectories. |
| `-r` | Reverse sorting order. |
| `-S` | Sort by file size, largest first. |
| `-s` | Display allocated block counts. |
| `-t` | Sort by the selected time field. |
| `-u` | Use access time for relevant time operations. |
| `-w` | Print non-printable filename characters without replacement. |

The implementation also handles option precedence for selected opposing options. Exact compatibility with the original utility for every combination and filesystem edge case has not been established.

## 8. Testing

The program has been compiled successfully on NetBSD 10.1 using `make clean` and `make`. The build completed without compiler warnings in the recorded test run.

The following behaviors have been tested:

- Directory listing and hidden-entry options (`-a`, `-A`).
- Directory operand handling (`-d`).
- Recursive listing (`-R`).
- Long-format information (`-l`) and numeric IDs (`-n`).
- Inode numbers (`-i`).
- File-type indicators (`-F`).
- Sorting by size and time (`-S`, `-t`) and reverse sorting (`-r`).
- Block counts (`-s`) and size options (`-h`, `-k`).
- Non-printable filename handling (`-q`, `-w`).
- Symbolic-link target display.
- Error reporting for a nonexistent path and a nonzero exit status.
- Selected combinations of options.

The program's output has been compared with the system `ls` for selected test cases. Some output formatting and untested edge cases may differ from the original NetBSD implementation.

## 9. Limitations

This project implements a subset of NetBSD `ls(1)` for educational purposes. It has not been verified as fully compatible with every detail of the supplied manual, including every combination of options and all possible filesystem object types.

## 10. GitHub Repository

Repository URL:

https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm
