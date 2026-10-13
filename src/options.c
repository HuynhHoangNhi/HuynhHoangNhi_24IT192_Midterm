#include <stddef.h>
#include "options.h"

/*
 * Parse short options, including grouped options such as -al and -la.
 * Return 0 on success, or -1 when an unsupported option is encountered.
 */
int parse_options(int argc, char *argv[], Options *options)
{
    int i;
    int j;

    if (argc < 1 || argv == NULL || options == NULL) {
        return -1;
    }

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        /* A lone '-' is an operand, not an option. */
        if (arg == NULL || arg[0] != '-' || arg[1] == '\0') {
            continue;
        }

        /* Stop parsing options after the conventional "--" marker. */
        if (arg[1] == '-' && arg[2] == '\0') {
            break;
        }

        /* This implementation supports short options only. */
        if (arg[1] == '-') {
            return -1;
        }

        for (j = 1; arg[j] != '\0'; j++) {
            switch (arg[j]) {
            case 'a':
                options->show_all = 1;
                options->almost_all = 0;
                break;

            case 'A':
                options->almost_all = 1;
                options->show_all = 0;
                break;

            case 'c':
                options->use_change_time = 1;
                options->use_access_time = 0;
                break;

            case 'd':
                options->directory = 1;
                options->recursive = 0;
                break;

            case 'F':
                options->classify = 1;
                break;

            case 'f':
                options->no_sort = 1;
                options->show_all = 1;
                options->almost_all = 0;
                break;

            case 'h':
                options->human_readable = 1;
                options->kilobytes = 0;
                break;

            case 'i':
                options->show_inode = 1;
                break;

            case 'k':
                options->kilobytes = 1;
                options->human_readable = 0;
                break;

            case 'l':
                options->long_format = 1;
                options->numeric_ids = 0;
                break;

            case 'n':
                options->long_format = 1;
                options->numeric_ids = 1;
                break;

            case 'q':
                options->quote_nonprint = 1;
                options->raw_nonprint = 0;
                break;

            case 'R':
                options->recursive = 1;
                options->directory = 0;
                break;

            case 'r':
                options->reverse = 1;
                break;

            case 'S':
                options->sort_size = 1;
                options->sort_time = 0;
                break;

            case 's':
                options->show_blocks = 1;
                break;

            case 't':
                options->sort_time = 1;
                options->sort_size = 0;
                break;

            case 'u':
                options->use_access_time = 1;
                options->use_change_time = 0;
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

    return 0;
}