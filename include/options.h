
#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct {
    int show_all;       /* -a */
    int almost_all;     /* -A */
    int directory;      /* -d */
    int recursive;      /* -R */
    int reverse;        /* -r */
    int no_sort;        /* -f */
    int long_format;    /* -l */
    int numeric_ids;    /* -n */
    int show_inode;     /* -i */
    int classify;       /* -F */
    int sort_size;      /* -S */
    int sort_time;      /* -t */
    int use_access_time;/* -u */
    int use_change_time;/* -c */
    int human_readable; /* -h */
    int kilobytes;      /* -k */
    int show_blocks;    /* -s */
    int quote_nonprint; /* -q */
    int raw_nonprint;   /* -w */
} Options;

int parse_options(int argc, char *argv[], Options *options);

#endif

