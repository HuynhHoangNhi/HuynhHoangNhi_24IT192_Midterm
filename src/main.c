
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "options.h"
#include "list.h"
#include "display.h"

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
        fprintf(stderr, "Usage: ls [-aAcdFfhiklnqRrSstuw] [file ...]\n");
        return 1;
    }

    /*
     * Collect operands. This simplified implementation assumes
     * options are parsed by parse_options() before this stage.
     */
    for (i = 1; i < argc; i++) {
        if (!end_options && strcmp(argv[i], "--") == 0) {
            end_options = 1;
            continue;
        }

        if (!end_options && argv[i][0] == '-' && argv[i][1] != '\0') {
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
        struct stat metadata;
        FileEntry entry;
        FileEntry *entries = NULL;
        size_t count = 0;
        size_t j;

        if (lstat(paths[i], &metadata) == -1) {
            perror(paths[i]);
            exit_status = 1;
            continue;
        }

        if (!S_ISDIR(metadata.st_mode) || options.directory) {
            entry.name = (char *)paths[i];
            entry.metadata = metadata;
            display_entry(paths[i], &entry, &options);
            continue;
        }

        if (read_directory(paths[i], &options, &entries, &count) == -1) {
            perror(paths[i]);
            exit_status = 1;
            continue;
        }

        sort_entries(entries, count, &options);

        if (operand_count > 1 || options.recursive) {
            printf("%s:\n", paths[i]);
        }

        display_entries(paths[i], entries, count, &options);

        if (options.recursive) {
            for (j = 0; j < count; j++) {
                char subpath[4096];

                if (!S_ISDIR(entries[j].metadata.st_mode) ||
                    strcmp(entries[j].name, ".") == 0 ||
                    strcmp(entries[j].name, "..") == 0) {
                    continue;
                }

                if (snprintf(subpath, sizeof(subpath), "%s/%s",
                             paths[i], entries[j].name) >=
                    (int)sizeof(subpath)) {
                    fprintf(stderr, "ls: path too long: %s/%s\n",
                            paths[i], entries[j].name);
                    exit_status = 1;
                    continue;
                }

                printf("\n%s:\n", subpath);

                {
                    FileEntry *subentries = NULL;
                    size_t subcount = 0;

                    if (read_directory(subpath, &options,
                                       &subentries, &subcount) == -1) {
                        perror(subpath);
                        exit_status = 1;
                        continue;
                    }

                    sort_entries(subentries, subcount, &options);
                    display_entries(subpath, subentries,
                                    subcount, &options);
                    free_entries(subentries, subcount);
                }
            }
        }

        free_entries(entries, count);
    }

    return exit_status;
}
