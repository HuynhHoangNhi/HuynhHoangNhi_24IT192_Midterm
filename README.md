# Simplified NetBSD ls - Midterm Project

## 1. Project Information

- **Course:** Midterm Project
- **Operating System:** NetBSD 10.1
- **Programming Language:** C
- **Repository:** https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm

## 2. Project Description

This project implements a simplified version of the NetBSD `ls(1)` command in C. It is based on a subset of the options described in the supplied NetBSD manual.

The program supports directory listing, file information display, sorting, recursive directory traversal, and selected output-formatting options.

The source code is divided into multiple C source files and header files to separate option parsing, directory processing, and output formatting.

## 3. Project Structure

```text
ls-midterm/
|-- include/
|   |-- options.h
|   |-- list.h
|   `-- display.h
|-- src/
|   |-- main.c
|   |-- options.c
|   |-- list.c
|   `-- display.c
|-- Makefile
|-- README.md
`-- .gitignore
```

## 4. Compilation

Build the program on NetBSD using:

```sh
make
```

Remove generated object files and the executable:

```sh
make clean
```

## 5. Usage

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

## 6. Implemented Options

The implemented option set includes:

- `-a`: Include all entries, including `.` and `..`.
- `-A`: Include hidden entries except `.` and `..`.
- `-c`: Use status-change time for relevant time operations.
- `-d`: List directory entries themselves.
- `-F`: Append file-type indicators.
- `-f`: Disable sorting and include hidden entries.
- `-h`: Display sizes in human-readable units.
- `-i`: Display inode numbers.
- `-k`: Display sizes in 1024-byte units.
- `-l`: Use long-format output.
- `-n`: Display numeric user and group IDs.
- `-q`: Replace non-printable filename characters with `?`.
- `-R`: Recursively list subdirectories.
- `-r`: Reverse sorting order.
- `-S`: Sort by file size.
- `-s`: Display allocated block counts.
- `-t`: Sort by the selected time field.
- `-u`: Use access time for relevant time operations.
- `-w`: Display non-printable filename characters without replacing them.

This project implements a subset of NetBSD `ls(1)` and is not a complete replacement for the original command.

## 7. Testing

The program was compiled on NetBSD 10.1 using `make clean` and `make`.

The following behaviors have been tested:

- Listing directory contents.
- Displaying hidden entries with `-a` and `-A`.
- Displaying directories with `-d` and `-ld`.
- Recursive directory traversal.
- Long-format output with `-l`.
- Inode numbers with `-i`.
- Numeric user and group IDs with `-n`.
- File-type indicators with `-F`.
- Sorting by size and time with `-S` and `-t`.
- Reverse sorting with `-r`.
- Human-readable and kilobyte size output with `-h` and `-k`.
- Block counts with `-s`.
- Non-printable filename handling with `-q` and `-w`.
- Error handling for a nonexistent path, which returned a nonzero exit status.

Further testing is needed to establish compatibility with every detail of the original NetBSD manual.

## 8. Limitations

This is an educational implementation of a subset of NetBSD `ls(1)`. Behavior may differ from the original command in untested edge cases or combinations of options.

## 9. GitHub Repository

https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm
