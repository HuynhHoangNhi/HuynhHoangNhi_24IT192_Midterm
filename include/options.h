#ifndef OPTIONS_H
#define OPTIONS_H

/*
 * Command-line options supported by our simplified ls program.
 * Each field is non-zero when the corresponding behavior is enabled.
 */
typedef struct {
    int show_all;           /* -a: include entries beginning with '.' */
    int almost_all;         /* -A: include hidden entries except . and .. */
    int directory;          /* -d: list directories themselves */
    int recursive;          /* -R: list subdirectories recursively */
    int reverse;             /* -r: reverse the sort order */
    int no_sort;             /* -f: do not sort entries */

    int long_format;         /* -l or -n: use long listing format */
    int numeric_ids;         /* -n: display numeric user/group IDs */
    int show_inode;          /* -i: display inode numbers */
    int classify;            /* -F: append file-type indicators */
    int sort_size;           /* -S: sort by file size */
    int sort_time;           /* -t: sort by modification/access/change time */

    int use_access_time;     /* -u: use access time */
    int use_change_time;     /* -c: use status-change time */

    int human_readable;      /* -h: human-readable block sizes */
    int kilobytes;           /* -k: use 1024-byte block units */
    int show_blocks;         /* -s: display allocated block counts */

    int quote_nonprint;      /* -q: replace non-printable characters */
    int raw_nonprint;        /* -w: display non-printable characters as-is */
} Options;

/* Parse command-line options. Return 0 on success, -1 on an invalid option. */
int parse_options(int argc, char *argv[], Options *options);

#endif