
#include <string.h>
#include "options.h"

int parse_options(int argc, char *argv[], Options *options)
{
    int i;
    int j;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] == '\0') {
            continue;
        }

        for (j = 1; argv[i][j] != '\0'; j++) {
            switch (argv[i][j]) {
            case 'a':
                options->show_all = 1;
                break;
            case 'A':
                options->almost_all = 1;
                break;
            case 'd':
                options->directory = 1;
                break;
            case 'R':
                options->recursive = 1;
                break;
            case 'r':
                options->reverse = 1;
                break;
            case 'f':
                options->no_sort = 1;
                break;
            case 'l':
                options->long_format = 1;
                break;
            case 'n':
                options->numeric_ids = 1;
                break;
            case 'i':
                options->show_inode = 1;
                break;
            case 'F':
                options->classify = 1;
                break;
            case 'S':
                options->sort_size = 1;
                break;
            case 't':
                options->sort_time = 1;
                break;
            case 'u':
                options->use_access_time = 1;
                break;
            case 'c':
                options->use_change_time = 1;
                break;
            case 'h':
                options->human_readable = 1;
                options->kilobytes = 0;
                break;
            case 'k':
                options->kilobytes = 1;
                options->human_readable = 0;
                break;
            case 's':
                options->show_blocks = 1;
                break;
            case 'q':
                options->quote_nonprint = 1;
                options->raw_nonprint = 0;
                break;
            case 'w':
                options->raw_nonprint = 1;
                options->quote_nonprint = 0;
                break;
            default:
                return -1;
            }
        }
    }

    if (options->numeric_ids) {
        options->long_format = 1;
    }

    return 0;
}

