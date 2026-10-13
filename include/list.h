#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <sys/stat.h>
#include "options.h"

/*
 * Information about one filesystem entry.
 * The name is dynamically allocated and must be freed by the caller.
 */
typedef struct {
    char *name;
    struct stat metadata;
} FileEntry;

/*
 * Read entries from a directory.
 * Returns 0 on success and -1 on failure.
 * The caller owns the returned array and each entry name.
 */
int read_directory(const char *path,
                   const Options *options,
                   FileEntry **entries,
                   size_t *count);

/* Sort entries according to the selected options. */
void sort_entries(FileEntry *entries,
                  size_t count,
                  const Options *options);

/* Release the array and its entry names. */
void free_entries(FileEntry *entries, size_t count);

#endif