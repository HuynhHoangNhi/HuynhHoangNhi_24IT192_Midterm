
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "options.h"
#include "list.h"
#include "display.h"

/* List a path and recursively visit its subdirectories when -R is set. */
static int list_path(const char *path,
                     const Options *options,
                     int show_heading)
{
    struct stat metadata;
    FileEntry entry;
    FileEntry *entries = NULL;
    size_t count = 0;
    size_t i;
    int exit_status = 0;

    if (lstat(path, &metadata) == -1) {
        perror(path);
        return 1;
    }

    /* Display a file or display the directory itself when -d is selected. */
    if (!S_ISDIR(metadata.st_mode) || options->directory) {
        entry.name = (char *)path;
        entry.metadata = metadata;
        display_entry(path, &entry, options);
        return 0;
    }

    if (read_directory(path, options, &entries, &count) == -1) {
        perror(path);
        return 1;
    }

    sort_entries(entries, count, options);

    if (show_heading) {
        printf("%s:\n", path);
    }

    display_entries(path, entries, count, options);

    /* Recursively visit every subdirectory when -R is selected. */
    if (options->recursive) {
        for (i = 0; i < count; i++) {
            char *subpath;
            size_t path_length;
            size_t name_length;
            size_t subpath_length;
            int needs_separator;

            if (!S_ISDIR(entries[i].metadata.st_mode) ||
                strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0) {
                continue;
            }

            path_length = strlen(path);
            name_length = strlen(entries[i].name);
            needs_separator =
                path_length > 0 && path[path_length - 1] != '/';

            subpath_length = path_length + name_length
                           + (size_t)needs_separator + 1;

            subpath = malloc(subpath_length);
            if (subpath == NULL) {
                fprintf(stderr, "ls: cannot allocate memory\n");
                exit_status = 1;
                continue;
            }

            if (snprintf(subpath, subpath_length, "%s%s%s",
                         path,
                         needs_separator ? "/" : "",
                         entries[i].name) >= (int)subpath_length) {
                fprintf(stderr, "ls: path too long\n");
                free(subpath);
                exit_status = 1;
                continue;
            }

            printf("\n");
            if (list_path(subpath, options, 1) != 0) {
                exit_status = 1;
            }

            free(subpath);
        }
    }

    free_entries(entries, count);
    return exit_status;
}

int main(int argc, char *argv[])
{
    Options options = {0};
    int option_result;
    int i;
    int operand_count = 0;
    int exit_status = 0;
    int end_options = 0;
    const char *paths[1024];

    option_result = parse_options(argc, argv, &options);
    if (option_result != 0) {
        fprintf(stderr, "ls: invalid or unsupported option\n");
        fprintf(stderr,
                "Usage: ls [-aAcdFfhiklnqRrSstuw] [file ...]\n");
        return 1;
    }

    /* Collect file and directory operands. */
    for (i = 1; i < argc; i++) {
        if (!end_options && strcmp(argv[i], "--") == 0) {
            end_options = 1;
            continue;
        }

        if (!end_options && argv[i][0] == '-' &&
            argv[i][1] != '\0') {
            continue;
        }

        if (operand_count < 1024) {
            paths[operand_count++] = argv[i];
        } else {
            fprintf(stderr, "ls: too many operands\n");
            return 1;
        }
    }

    if (operand_count == 0) {
        paths[operand_count++] = ".";
    }

    for (i = 0; i < operand_count; i++) {
        int show_heading =
            !options.directory &&
            (operand_count > 1 || options.recursive);

        if (list_path(paths[i], &options, show_heading) != 0) {
            exit_status = 1;
        }
    }

    return exit_status;
}