
# Simplified NetBSD ls - Midterm Project

## 1. Project Information

- **Course:** Midterm Project
- **Operating System:** NetBSD
- **Programming Language:** C
- **Repository:** https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm

## 2. Project Description

This project implements a simplified version of the UNIX `ls` command in C, based on the options and requirements specified in the provided NetBSD `ls(1)` manual.

The program is organized into multiple source files and header files to separate command-line option parsing, directory processing, and output formatting.

## 3. Project Structure

```text
ls-midterm/
├── include/
│   ├── options.h
│   ├── list.h
│   └── display.h
├── src/
│   ├── main.c
│   ├── options.c
│   ├── list.c
│   └── display.c
├── Makefile
├── README.md
└── .gitignore
```

## 4. Compilation

On a NetBSD system with a C compiler and `make` installed, run:

```sh
make
```

To remove generated object files and the executable, run:

```sh
make clean
```

## 5. Usage

After successful compilation, run:

```sh
./ls
./ls .
./ls -a
./ls -l
./ls -i
./ls -R
```

The supported options and their actual behavior will be documented after compilation and testing.

## 6. Testing

Compilation results, test commands, expected output, and observed output will be added after testing on NetBSD.

## 7. Limitations

This is an educational implementation. Its compatibility with the original NetBSD `ls(1)` command must be evaluated through testing against the supplied manual.

## 8. GitHub Repository

https://github.com/HuynhHoangNhi/HuynhHoangNhi_24IT192_Midterm
